#pragma once

#include "../../models.h"

#include <QWidget>

class QLineEdit;
class QComboBox;
class QSpinBox;
class QPlainTextEdit;
class QPushButton;
class QLabel;
class CppSyntaxHighlighter;

class SolutionEditorView : public QWidget
{
    Q_OBJECT

public:
    explicit SolutionEditorView(QWidget *parent = nullptr);

    void loadSolution(const Solution &solution);
    void setCreateMode();
    void setKnownPlatforms(const QList<QString> &platforms);

signals:
    void saved(const Solution &solution);

private slots:
    void onSaveClicked();

private:
    void setupUI();
    void clearForm();
    Solution collectSolution() const;

    int m_currentId = -1;

    QLabel         *m_modeLabel;
    QLineEdit      *m_titleEdit;
    QComboBox      *m_platformCombo;
    QSpinBox       *m_difficultyEdit;
    QLineEdit      *m_urlEdit;
    QLineEdit      *m_topicsEdit;
    QPlainTextEdit *m_codeEditor;
    QPushButton    *m_saveBtn;

    CppSyntaxHighlighter *m_highlighter = nullptr;
};