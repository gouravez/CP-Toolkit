#pragma once

#include <QObject>
#include <QString>
#include <QTemporaryDir>
#include <atomic>

class StressTestRunner : public QObject
{
    Q_OBJECT

public:
    enum class Verdict {
        Running,
        Accepted,
        Mismatch,
        TimeLimitExceeded,
        CompileError,
        RuntimeError,
        Stopped
    };

    struct IterationResult {
        int     iteration = 0;
        Verdict verdict   = Verdict::Running;
        QString input;
        QString bruteOutput;
        QString optimizedOutput;
        QString diff;
        QString errorMessage;
    };

    explicit StressTestRunner(QObject *parent = nullptr);

    void setSources(const QString &generatorPath,
                    const QString &brutePath,
                    const QString &optimizedPath);

    void setTimeLimitMs(int ms);

    void setMaxIterations(int iterations);
    QString tempDirectory() const;

    bool isRunning() const;


public slots:
    void start();
    void stop();

signals:
    void iterationFinished(const IterationResult &result);
    void finished(const IterationResult &result);
    void compileError(const QString &target, const QString &output);

private:
    bool compileSource(const QString &sourcePath,
                       const QString &outputPath,
                       QString &errorOut);

    bool runWithTimeout(const QString &exePath,
                        const QString &input,
                        QString &stdoutOut,
                        QString &stderrOut,
                        bool &timedOut);

    QString m_generatorPath;
    QString m_brutePath;
    QString m_optimizedPath;

    int  m_timeLimitMs = 2000;
    std::atomic<bool> m_running{false};

    QTemporaryDir m_tempDir;

    std::atomic<bool> m_stopRequested{false};
    int  m_currentIteration = 0;
    int  m_maxIterations    = 1000;
    QString m_generatorExe;
    QString m_bruteExe;
    QString m_optimizedExe;

    QString makeDiff(const QString &expected, const QString &actual) const;
    QString tempBinaryPath(const QString &label) const;
};