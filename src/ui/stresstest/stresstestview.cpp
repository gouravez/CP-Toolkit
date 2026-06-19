#include "stresstestview.h"
#include "../editor/Cppsyntaxhighlighter.h"

#include <QDir>
#include <QFile>
#include <QFileDialog>
#include <QFileInfo>
#include <QFont>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QLabel>
#include <QMetaObject>
#include <QPlainTextEdit>
#include <QPushButton>
#include <QSpinBox>
#include <QTextEdit>
#include <QThread>
#include <QVBoxLayout>

StressTestView::StressTestView(QWidget *parent)
    : QWidget(parent)
{
    setObjectName("StressTestView");
    setupUI();
}

StressTestView::~StressTestView()
{
    if (m_runner && m_runner->isRunning())
        m_runner->stop();

    if (m_workerThread) {
        m_workerThread->quit();
        m_workerThread->wait();
    }
}

StressTestView::SourcePane StressTestView::buildSourcePane(QWidget *parent, const QString &title)
{
    SourcePane pane;

    pane.pathLabel = new QLabel(tr("No file selected"), parent);
    pane.pathLabel->setObjectName("PathLabel");
    pane.pathLabel->setWordWrap(true);

    pane.browseBtn = new QPushButton(tr("Browse..."), parent);
    pane.browseBtn->setObjectName("SecondaryButton");
    pane.browseBtn->setCursor(Qt::PointingHandCursor);

    pane.editor = new QPlainTextEdit(parent);
    pane.editor->setObjectName("CodeEditor");
    pane.editor->setPlaceholderText(
        tr("Type or paste the %1 source here, or browse a file...").arg(title.toLower()));
    pane.editor->setLineWrapMode(QPlainTextEdit::NoWrap);

    QFont monoFont(QStringLiteral("Consolas"));
    monoFont.setStyleHint(QFont::Monospace);
    monoFont.setPointSize(10);
    pane.editor->setFont(monoFont);

    return pane;
}

void StressTestView::setupUI()
{
    QVBoxLayout *root = new QVBoxLayout(this);
    root->setContentsMargins(16, 16, 16, 16);
    root->setSpacing(10);

    QLabel *header = new QLabel(tr("Stress Test"), this);
    header->setObjectName("PanelTitle");
    root->addWidget(header);

    QHBoxLayout *panesLayout = new QHBoxLayout;
    panesLayout->setSpacing(10);

    auto addPane = [&](SourcePane &pane, const QString &title) {
        QGroupBox *box = new QGroupBox(title, this);
        box->setObjectName("SourcePaneBox");

        pane = buildSourcePane(box, title);

        QHBoxLayout *pathRow = new QHBoxLayout;
        pathRow->addWidget(pane.pathLabel, 1);
        pathRow->addWidget(pane.browseBtn);

        QVBoxLayout *boxLayout = new QVBoxLayout(box);
        boxLayout->addLayout(pathRow);
        boxLayout->addWidget(pane.editor, 1);

        panesLayout->addWidget(box, 1);
    };

    addPane(m_generatorPane, tr("Generator"));
    addPane(m_brutePane,     tr("Brute Force"));
    addPane(m_optimizedPane, tr("Optimized"));

    root->addLayout(panesLayout, 3);

    m_genHighlighter   = new CppSyntaxHighlighter(m_generatorPane.editor->document());
    m_bruteHighlighter = new CppSyntaxHighlighter(m_brutePane.editor->document());
    m_optHighlighter   = new CppSyntaxHighlighter(m_optimizedPane.editor->document());

    QHBoxLayout *controls = new QHBoxLayout;
    controls->setSpacing(8);

    controls->addWidget(new QLabel(tr("Time limit (ms):"), this));
    m_timeLimitSpin = new QSpinBox(this);
    m_timeLimitSpin->setObjectName("FormInput");
    m_timeLimitSpin->setRange(100, 60000);
    m_timeLimitSpin->setSingleStep(100);
    m_timeLimitSpin->setValue(2000);
    controls->addWidget(m_timeLimitSpin);

    controls->addSpacing(16);

    controls->addWidget(new QLabel(tr("Max iterations:"), this));
    m_maxIterationsSpin = new QSpinBox(this);
    m_maxIterationsSpin->setObjectName("FormInput");
    m_maxIterationsSpin->setRange(1, 1000000);
    m_maxIterationsSpin->setValue(1000);
    controls->addWidget(m_maxIterationsSpin);

    controls->addStretch();

    m_runBtn = new QPushButton(tr("Run"), this);
    m_runBtn->setObjectName("PrimaryButton");
    m_runBtn->setCursor(Qt::PointingHandCursor);

    m_stopBtn = new QPushButton(tr("Stop"), this);
    m_stopBtn->setObjectName("DangerButton");
    m_stopBtn->setCursor(Qt::PointingHandCursor);
    m_stopBtn->setEnabled(false);

    m_clearBtn = new QPushButton(tr("Clear All"), this);
    m_clearBtn->setObjectName("SecondaryButton");
    m_clearBtn->setCursor(Qt::PointingHandCursor);
    connect(m_clearBtn, &QPushButton::clicked, this, &StressTestView::onClearClicked);

    controls->addWidget(m_clearBtn);
    controls->addWidget(m_runBtn);
    controls->addWidget(m_stopBtn);

    root->addLayout(controls);

    m_statusLabel = new QLabel(tr("Idle"), this);
    m_statusLabel->setObjectName("StatusLabel");
    root->addWidget(m_statusLabel);

    m_outputLog = new QTextEdit(this);
    m_outputLog->setObjectName("OutputLog");
    m_outputLog->setReadOnly(true);

    QFont logFont(QStringLiteral("Consolas"));
    logFont.setStyleHint(QFont::Monospace);
    logFont.setPointSize(10);
    m_outputLog->setFont(logFont);

    root->addWidget(m_outputLog, 2);

    m_runner = std::make_unique<StressTestRunner>();

    m_workerThread = new QThread(this);
    m_runner->moveToThread(m_workerThread);
    m_workerThread->start();

    connect(m_generatorPane.browseBtn, &QPushButton::clicked, this, &StressTestView::onBrowseGenerator);
    connect(m_brutePane.browseBtn,     &QPushButton::clicked, this, &StressTestView::onBrowseBrute);
    connect(m_optimizedPane.browseBtn, &QPushButton::clicked, this, &StressTestView::onBrowseOptimized);

    connect(m_runBtn,  &QPushButton::clicked, this, &StressTestView::onRunClicked);
    connect(m_stopBtn, &QPushButton::clicked, this, &StressTestView::onStopClicked);

    connect(m_runner.get(), &StressTestRunner::iterationFinished,
            this,            &StressTestView::onIterationFinished);
    connect(m_runner.get(), &StressTestRunner::finished,
            this,            &StressTestView::onFinished);
    connect(m_runner.get(), &StressTestRunner::compileError,
            this,            &StressTestView::onCompileError);
}

void StressTestView::browseInto(SourcePane &pane)
{
    const QString path = QFileDialog::getOpenFileName(
        this, tr("Select C++ Source File"), QString(),
        tr("C++ Source Files (*.cpp *.cc *.cxx);;All Files (*)"));

    if (path.isEmpty())
        return;

    QFile file(path);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        appendLog(tr("Could not open %1: %2").arg(path, file.errorString()), QStringLiteral("#e06c75"));
        return;
    }

    const QString contents = QString::fromUtf8(file.readAll());
    file.close();

    pane.filePath = path;
    pane.pathLabel->setText(QFileInfo(path).fileName());
    pane.pathLabel->setToolTip(path);
    pane.editor->setPlainText(contents);
}

bool StressTestView::flushPaneToDisk(SourcePane &pane, const QString &label, QString &errorOut)
{
    if (pane.editor->toPlainText().trimmed().isEmpty()) {
        errorOut = tr("%1 source is empty.").arg(label);
        return false;
    }

    if (pane.filePath.isEmpty()) {
        const QString dir = m_runner->tempDirectory();
        pane.filePath = QDir(dir).filePath(QStringLiteral("cpt_stress_%1.cpp").arg(label));
        pane.pathLabel->setText(tr("(unsaved) %1").arg(QFileInfo(pane.filePath).fileName()));
        pane.pathLabel->setToolTip(pane.filePath);
    }

    QFile file(pane.filePath);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Truncate | QIODevice::Text)) {
        errorOut = tr("Could not write %1: %2").arg(pane.filePath, file.errorString());
        return false;
    }

    file.write(pane.editor->toPlainText().toUtf8());
    file.close();
    return true;
}

void StressTestView::onBrowseGenerator() { browseInto(m_generatorPane); }
void StressTestView::onBrowseBrute()     { browseInto(m_brutePane); }
void StressTestView::onBrowseOptimized() { browseInto(m_optimizedPane); }

void StressTestView::onRunClicked()
{
    if (m_runner->isRunning())
        return;

    m_outputLog->clear();

    QString err;
    if (!flushPaneToDisk(m_generatorPane, QStringLiteral("gen"), err) ||
        !flushPaneToDisk(m_brutePane,     QStringLiteral("brute"), err) ||
        !flushPaneToDisk(m_optimizedPane, QStringLiteral("opt"), err)) {
        appendLog(err, QStringLiteral("#e06c75"));
        return;
    }

    m_runner->setSources(m_generatorPane.filePath, m_brutePane.filePath, m_optimizedPane.filePath);
    m_runner->setTimeLimitMs(m_timeLimitSpin->value());
    m_runner->setMaxIterations(m_maxIterationsSpin->value());

    setRunningState(true);
    m_statusLabel->setText(tr("Compiling..."));
    appendLog(tr("Starting stress test run..."));

    QMetaObject::invokeMethod(m_runner.get(), &StressTestRunner::start,
                               Qt::QueuedConnection);
}

void StressTestView::onStopClicked()
{
    m_statusLabel->setText(tr("Stopping..."));
    m_runner->stop();
}

void StressTestView::onIterationFinished(const StressTestRunner::IterationResult &result)
{
    if (result.verdict == StressTestRunner::Verdict::Accepted) {
        m_statusLabel->setText(tr("Running... iteration %1").arg(result.iteration));
        appendLog(tr("Iteration %1: OK").arg(result.iteration), QStringLiteral("#98c379"));
    }
}

void StressTestView::onFinished(const StressTestRunner::IterationResult &result)
{
    setRunningState(false);

    const QString verdictStr = verdictToString(result.verdict);

    switch (result.verdict) {
    case StressTestRunner::Verdict::Accepted:
        m_statusLabel->setText(tr("%1 -- %2 iterations, no mismatches found")
                                    .arg(verdictStr).arg(result.iteration));
        appendLog(tr("\n%1 iterations completed with no mismatches.").arg(result.iteration),
                  QStringLiteral("#98c379"));
        break;

    case StressTestRunner::Verdict::Mismatch:
        m_statusLabel->setText(tr("%1 at iteration %2").arg(verdictStr).arg(result.iteration));
        appendLog(tr("\n%1 at iteration %2!\n\nInput:\n%3")
                      .arg(verdictStr).arg(result.iteration).arg(result.input),
                  QStringLiteral("#e06c75"));
        appendLog(tr("Brute-force output:\n%1").arg(result.bruteOutput));
        appendLog(tr("Optimized output:\n%1").arg(result.optimizedOutput));
        appendLog(tr("Diff:\n%1").arg(result.diff), QStringLiteral("#e5c07b"));
        break;

    case StressTestRunner::Verdict::TimeLimitExceeded:
    case StressTestRunner::Verdict::RuntimeError:
        m_statusLabel->setText(tr("%1 at iteration %2").arg(verdictStr).arg(result.iteration));
        appendLog(QStringLiteral("\n") + result.errorMessage, QStringLiteral("#e06c75"));
        break;

    case StressTestRunner::Verdict::CompileError:
        m_statusLabel->setText(verdictStr);
        break;

    case StressTestRunner::Verdict::Stopped:
        m_statusLabel->setText(tr("%1 after %2 iterations").arg(verdictStr).arg(result.iteration));
        appendLog(tr("\nStopped by user after %1 iterations.").arg(result.iteration));
        break;

    case StressTestRunner::Verdict::Running:
        break;
    }
}

void StressTestView::onCompileError(const QString &target, const QString &compilerOutput)
{
    appendLog(tr("Compilation failed for %1:\n%2").arg(target, compilerOutput),
              QStringLiteral("#e06c75"));
}

void StressTestView::setRunningState(bool running)
{
    m_runBtn->setEnabled(!running);
    m_stopBtn->setEnabled(running);

    m_timeLimitSpin->setEnabled(!running);
    m_maxIterationsSpin->setEnabled(!running);

    m_generatorPane.browseBtn->setEnabled(!running);
    m_brutePane.browseBtn->setEnabled(!running);
    m_optimizedPane.browseBtn->setEnabled(!running);

    m_generatorPane.editor->setReadOnly(running);
    m_brutePane.editor->setReadOnly(running);
    m_optimizedPane.editor->setReadOnly(running);
}

void StressTestView::appendLog(const QString &text, const QString &color)
{
    QString html = text.toHtmlEscaped();
    html.replace(QLatin1Char('\n'), QStringLiteral("<br>"));

    if (!color.isEmpty())
        html = QStringLiteral("<span style=\"color:%1;\">%2</span>").arg(color, html);

    m_outputLog->append(html);
}

QString StressTestView::verdictToString(StressTestRunner::Verdict verdict) const
{
    switch (verdict) {
    case StressTestRunner::Verdict::Running:            return tr("Running");
    case StressTestRunner::Verdict::Accepted:           return tr("Accepted");
    case StressTestRunner::Verdict::Mismatch:           return tr("Mismatch");
    case StressTestRunner::Verdict::TimeLimitExceeded:  return tr("Time Limit Exceeded");
    case StressTestRunner::Verdict::CompileError:       return tr("Compile Error");
    case StressTestRunner::Verdict::RuntimeError:       return tr("Runtime Error");
    case StressTestRunner::Verdict::Stopped:            return tr("Stopped");
    }
    return tr("Unknown");
}

void StressTestView::onClearClicked()
{
    if (m_runner->isRunning())
        return;

    m_generatorPane.editor->clear();
    m_generatorPane.filePath.clear();
    m_generatorPane.pathLabel->setText(tr("No file selected"));
    m_generatorPane.pathLabel->setToolTip("");

    m_brutePane.editor->clear();
    m_brutePane.filePath.clear();
    m_brutePane.pathLabel->setText(tr("No file selected"));
    m_brutePane.pathLabel->setToolTip("");

    m_optimizedPane.editor->clear();
    m_optimizedPane.filePath.clear();
    m_optimizedPane.pathLabel->setText(tr("No file selected"));
    m_optimizedPane.pathLabel->setToolTip("");

    m_outputLog->clear();
    m_statusLabel->setText(tr("Idle"));
}