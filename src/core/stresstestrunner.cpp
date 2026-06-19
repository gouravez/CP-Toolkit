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
    compiler.waitForFinished(15000);
    errorOut = compiler.readAllStandardError();
    return compiler.exitCode() == 0;
}

bool StressTestRunner::runWithTimeout(const QString &exePath,
                                       const QString &input,
                                       QString &stdoutOut,
                                       QString &stderrOut,
                                       bool &timedOut)
{
    QProcess proc;
    proc.start(exePath, {});
    proc.waitForStarted();
    proc.write(input.toUtf8());
    proc.closeWriteChannel();

    timedOut = !proc.waitForFinished(m_timeLimitMs);
    if (timedOut) {
        proc.kill();
        return false;
    }

    stdoutOut = QString::fromUtf8(proc.readAllStandardOutput());
    stderrOut = QString::fromUtf8(proc.readAllStandardError());
    return proc.exitCode() == 0;
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

        QString genOut, genErr;
        bool timedOut = false;

        // pass iteration as argv[1] so generator can seed rng deterministically
        // e.g. mt19937 rng(atoi(argv[1]));
        QProcess genProc;
        genProc.start(m_generatorExe, {QString::number(m_currentIteration)});
        genProc.waitForStarted();
        genProc.closeWriteChannel();
        genProc.waitForFinished(m_timeLimitMs);
        genOut = QString::fromUtf8(genProc.readAllStandardOutput());
        
        QString bruteOut, bruteErr;
        bool bruteTLE = false;
        runWithTimeout(m_bruteExe, genOut, bruteOut, bruteErr, bruteTLE);

        QString optOut, optErr;
        bool optTLE = false;
        runWithTimeout(m_optimizedExe, genOut, optOut, optErr, optTLE);

        IterationResult result;
        result.iteration       = m_currentIteration;
        result.input           = genOut;
        result.bruteOutput     = bruteOut;
        result.optimizedOutput = optOut;

        if (optTLE) {
            result.verdict      = Verdict::TimeLimitExceeded;
            result.errorMessage = "Optimized solution exceeded time limit.";
            emit iterationFinished(result);
            emit finished(result);
            m_running = false;
            return;
        }

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