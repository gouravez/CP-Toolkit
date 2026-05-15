#include "stresstestrunner.h"

#include <QProcess>
#include <QFile>
#include <QTextStream>

StressTestRunner::StressTestRunner(QObject *parent)
    : QObject(parent)
{}

void StressTestRunner::setSources(const QString &generatorPath,
                                   const QString &brutePath,
                                   const QString &optimizedPath)
{
    m_generatorPath  = generatorPath;
    m_brutePath      = brutePath;
    m_optimizedPath  = optimizedPath;
}

void StressTestRunner::setTimeLimitMs(int ms)
{
    m_timeLimitMs = ms;
}

bool StressTestRunner::isRunning() const
{
    return m_running;
}

void StressTestRunner::stop()
{
    m_running = false;
}

bool StressTestRunner::compileSource(const QString &sourcePath,
                                      const QString &outputPath,
                                      QString &errorOut)
{
    QProcess compiler;
    compiler.start("g++", {"-O2", "-o", outputPath, sourcePath});
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

    stdoutOut = proc.readAllStandardOutput();
    stderrOut = proc.readAllStandardError();
    return proc.exitCode() == 0;
}

void StressTestRunner::start()
{
    if (m_running)
        return;

    m_running = true;

    QString genExe  = m_tempDir.filePath("gen");
    QString bruteExe = m_tempDir.filePath("brute");
    QString optExe  = m_tempDir.filePath("opt");

    QString compileError;

    if (!compileSource(m_generatorPath, genExe, compileError)) {
        emit this->compileError("generator", compileError);
        m_running = false;
        return;
    }

    if (!compileSource(m_brutePath, bruteExe, compileError)) {
        emit this->compileError("brute", compileError);
        m_running = false;
        return;
    }

    if (!compileSource(m_optimizedPath, optExe, compileError)) {
        emit this->compileError("optimized", compileError);
        m_running = false;
        return;
    }

    int iteration = 0;

    while (m_running) {
        iteration++;

        QString genStdout, genStderr;
        bool timedOut = false;

        runWithTimeout(genExe,
                       QString::number(iteration),
                       genStdout, genStderr, timedOut);

        QString bruteOut, bruteErr;
        runWithTimeout(bruteExe, genStdout, bruteOut, bruteErr, timedOut);

        QString optOut, optErr;
        runWithTimeout(optExe, genStdout, optOut, optErr, timedOut);

        IterationResult result;
        result.iteration      = iteration;
        result.input          = genStdout;
        result.bruteOutput    = bruteOut;
        result.optimizedOutput = optOut;

        if (bruteOut.trimmed() == optOut.trimmed()) {
            result.verdict = Verdict::Accepted;
        } else {
            result.verdict = Verdict::Mismatch;
            emit iterationFinished(result);
            emit finished(result);
            m_running = false;
            return;
        }

        emit iterationFinished(result);
    }

    IterationResult final;
    final.iteration = iteration;
    final.verdict   = Verdict::Accepted;
    emit finished(final);
    m_running = false;
}