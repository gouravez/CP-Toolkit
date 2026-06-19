#include "stresstestrunner.h"

#include <QProcess>
#include <QFile>
#include <QTextStream>
#include <QStringList>

StressTestRunner::StressTestRunner(QObject *parent)
    : QObject(parent)
{}

void StressTestRunner::setSources(const QString &generatorPath,
                                   const QString &brutePath,
                                   const QString &optimizedPath)
{
    m_generatorPath = generatorPath;
    m_brutePath     = brutePath;
    m_optimizedPath = optimizedPath;
}

void StressTestRunner::setTimeLimitMs(int ms)
{
    m_timeLimitMs = ms;
}

void StressTestRunner::setMaxIterations(int iterations)
{
    m_maxIterations = iterations;
}

QString StressTestRunner::tempDirectory() const
{
    return m_tempDir.path();
}

bool StressTestRunner::isRunning() const
{
    return m_running;
}

void StressTestRunner::stop()
{
    m_stopRequested = true;
}

bool StressTestRunner::compileSource(const QString &sourcePath,
                                      const QString &outputPath,
                                      QString &errorOut)
{
    QProcess compiler;
    compiler.start("g++", {"-O2", "-std=c++17", "-o", outputPath, sourcePath});

    if (!compiler.waitForStarted()) {
        errorOut = "Could not start 'g++'. Make sure a C++ compiler is "
                   "installed and available on your PATH.";
        return false;
    }

    compiler.waitForFinished(15000);
    errorOut = compiler.readAllStandardError();

    return compiler.exitStatus() == QProcess::NormalExit
        && compiler.exitCode()   == 0;
}

bool StressTestRunner::runWithTimeout(const QString &exePath,
                                       const QStringList &args,
                                       const QString &input,
                                       QString &stdoutOut,
                                       QString &stderrOut,
                                       bool &timedOut,
                                       bool &failedToStart)
{
    timedOut      = false;
    failedToStart = false;

    QProcess proc;
    proc.start(exePath, args);

    if (!proc.waitForStarted()) {
        failedToStart = true;
        return false;
    }

    proc.write(input.toUtf8());
    proc.closeWriteChannel();

    timedOut = !proc.waitForFinished(m_timeLimitMs);
    if (timedOut) {
        proc.kill();
        proc.waitForFinished();
        return false;
    }

    stdoutOut = QString::fromUtf8(proc.readAllStandardOutput());
    stderrOut = QString::fromUtf8(proc.readAllStandardError());

    return proc.exitStatus() == QProcess::NormalExit
        && proc.exitCode()   == 0;
}

QString StressTestRunner::makeDiff(const QString &expected,
                                    const QString &actual) const
{
    QStringList expLines = expected.trimmed().split('\n');
    QStringList actLines = actual.trimmed().split('\n');

    QString diff;
    int maxLines = qMax(expLines.size(), actLines.size());

    for (int i = 0; i < maxLines; i++) {
        QString e = i < expLines.size() ? expLines[i] : "<missing>";
        QString a = i < actLines.size() ? actLines[i] : "<missing>";
        if (e != a)
            diff += QString("Line %1:\n  expected: %2\n  got:      %3\n")
                        .arg(i + 1).arg(e).arg(a);
    }

    return diff.isEmpty() ? "(outputs match after trim)" : diff;
}

QString StressTestRunner::tempBinaryPath(const QString &label) const
{
    return m_tempDir.filePath(label);
}

void StressTestRunner::start()
{
    if (m_running)
        return;

    m_running        = true;
    m_stopRequested  = false;
    m_currentIteration = 0;

    QString compileError;

    m_generatorExe = tempBinaryPath("gen");
    m_bruteExe     = tempBinaryPath("brute");
    m_optimizedExe = tempBinaryPath("opt");

    if (!compileSource(m_generatorPath, m_generatorExe, compileError)) {
        emit this->compileError("generator", compileError);
        m_running = false;
        return;
    }

    if (!compileSource(m_brutePath, m_bruteExe, compileError)) {
        emit this->compileError("brute", compileError);
        m_running = false;
        return;
    }

    if (!compileSource(m_optimizedPath, m_optimizedExe, compileError)) {
        emit this->compileError("optimized", compileError);
        m_running = false;
        return;
    }

    while (m_running && !m_stopRequested
           && m_currentIteration < m_maxIterations)
    {
        m_currentIteration++;

        // pass iteration as argv[1] so generator can seed rng deterministically
        // e.g. mt19937 rng(atoi(argv[1]));
        QString genOut, genErr;
        bool genTimedOut = false, genFailedToStart = false;
        const bool genOk = runWithTimeout(
            m_generatorExe, {QString::number(m_currentIteration)}, QString(),
            genOut, genErr, genTimedOut, genFailedToStart);

        if (!genOk) {
            IterationResult result;
            result.iteration = m_currentIteration;

            if (genTimedOut) {
                result.verdict      = Verdict::TimeLimitExceeded;
                result.errorMessage = "Generator exceeded the time limit.";
            } else if (genFailedToStart) {
                result.verdict      = Verdict::RuntimeError;
                result.errorMessage = "Generator could not be started.";
            } else {
                result.verdict      = Verdict::RuntimeError;
                result.errorMessage = "Generator exited with an error:\n" + genErr;
            }

            emit iterationFinished(result);
            emit finished(result);
            m_running = false;
            return;
        }

        QString bruteOut, bruteErr;
        bool bruteTimedOut = false, bruteFailedToStart = false;
        const bool bruteOk = runWithTimeout(
            m_bruteExe, {}, genOut, bruteOut, bruteErr, bruteTimedOut, bruteFailedToStart);

        if (!bruteOk) {
            IterationResult result;
            result.iteration = m_currentIteration;
            result.input     = genOut;

            if (bruteTimedOut) {
                result.verdict      = Verdict::TimeLimitExceeded;
                result.errorMessage = "Brute-force solution exceeded the time limit.";
            } else if (bruteFailedToStart) {
                result.verdict      = Verdict::RuntimeError;
                result.errorMessage = "Brute-force solution could not be started.";
            } else {
                result.verdict      = Verdict::RuntimeError;
                result.errorMessage = "Brute-force solution exited with an error:\n" + bruteErr;
            }

            emit iterationFinished(result);
            emit finished(result);
            m_running = false;
            return;
        }

        QString optOut, optErr;
        bool optTimedOut = false, optFailedToStart = false;
        const bool optOk = runWithTimeout(
            m_optimizedExe, {}, genOut, optOut, optErr, optTimedOut, optFailedToStart);

        if (!optOk) {
            IterationResult result;
            result.iteration   = m_currentIteration;
            result.input       = genOut;
            result.bruteOutput = bruteOut;

            if (optTimedOut) {
                result.verdict      = Verdict::TimeLimitExceeded;
                result.errorMessage = "Optimized solution exceeded the time limit.";
            } else if (optFailedToStart) {
                result.verdict      = Verdict::RuntimeError;
                result.errorMessage = "Optimized solution could not be started.";
            } else {
                result.verdict      = Verdict::RuntimeError;
                result.errorMessage = "Optimized solution exited with an error:\n" + optErr;
            }

            emit iterationFinished(result);
            emit finished(result);
            m_running = false;
            return;
        }

        IterationResult result;
        result.iteration       = m_currentIteration;
        result.input           = genOut;
        result.bruteOutput     = bruteOut;
        result.optimizedOutput = optOut;

        if (bruteOut.trimmed() != optOut.trimmed()) {
            result.verdict = Verdict::Mismatch;
            result.diff    = makeDiff(bruteOut, optOut);
            emit iterationFinished(result);
            emit finished(result);
            m_running = false;
            return;
        }

        result.verdict = Verdict::Accepted;
        emit iterationFinished(result);
    }

    IterationResult final;
    final.iteration = m_currentIteration;
    final.verdict   = m_stopRequested ? Verdict::Stopped : Verdict::Accepted;
    emit finished(final);
    m_running = false;
}