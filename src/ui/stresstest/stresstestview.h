#pragma once

#include <QWidget>
#include <memory>

#include "../../core/stresstestrunner.h"

class QPlainTextEdit;
class QPushButton;
class QLabel;
class QSpinBox;
class QTextEdit;
class CppSyntaxHighlighter;

class StressTestView : public QWidget
{
    Q_OBJECT

public:
    explicit StressTestView(QWidget *parent = nullptr);
    ~StressTestView() override;

private slots:
    void onBrowseGenerator();
    void onBrowseBrute();
    void onBrowseOptimized();

    void onRunClicked();
    void onStopClicked();

    void onIterationFinished(const StressTestRunner::IterationResult &result);
    void onFinished(const StressTestRunner::IterationResult &result);
    void onCompileError(const QString &target, const QString &compilerOutput);

private:
    struct SourcePane
    {
        QLabel         *pathLabel  = nullptr;
        QPushButton    *browseBtn  = nullptr;
        QPlainTextEdit *editor     = nullptr;
        QString         filePath;
    };

    void setupUI();
    SourcePane buildSourcePane(QWidget *parent, const QString &title);
    void browseInto(SourcePane &pane);

    bool flushPaneToDisk(SourcePane &pane, const QString &label, QString &errorOut);

    void setRunningState(bool running);
    void appendLog(const QString &text, const QString &color = QString());
    QString verdictToString(StressTestRunner::Verdict verdict) const;

    SourcePane m_generatorPane;
    SourcePane m_brutePane;
    SourcePane m_optimizedPane;

    QSpinBox *m_timeLimitSpin     = nullptr;
    QSpinBox *m_maxIterationsSpin = nullptr;

    QPushButton *m_runBtn  = nullptr;
    QPushButton *m_stopBtn = nullptr;

    QLabel    *m_statusLabel = nullptr;
    QTextEdit *m_outputLog   = nullptr;

    std::unique_ptr<StressTestRunner> m_runner;
    CppSyntaxHighlighter *m_genHighlighter   = nullptr;
    CppSyntaxHighlighter *m_bruteHighlighter = nullptr;
    CppSyntaxHighlighter *m_optHighlighter   = nullptr;
};