#pragma once

#include <QMainWindow>
#include <memory>

class Sidebar;
class QStackedWidget;
class QShortcut;

// DAL
class SqliteDatabase;
class SnippetRepository;
class SolutionRepository;

// Core
class SearchIndex;

// Views
class SnippetListView;
class SnippetEditorView;
class SolutionListView;
class SolutionEditorView;
class SearchView;
class StressTestView;
class WorkspaceView;
class TemplateView;

// Models
struct Snippet;
struct Solution;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void onSidebarItemSelected(int index);

    void onSnippetSelected(int id);
    void onSnippetCreateRequested();
    void onSnippetDeleteRequested(int id);
    void onSnippetSaved(const Snippet &snippet);

    void onSolutionSelected(int id);
    void onSolutionCreateRequested();
    void onSolutionDeleteRequested(int id);
    void onSolutionSaved(const Solution &solution);

    void onSnippetActivated(int id);
    void onSolutionActivated(int id);

    void onGlobalSearch();

private:
    void setupDatabase();
    void setupLayout();
    void setupShortcuts();
    void buildSearchIndex();

    void refreshSnippets();
    void refreshSolutions();

    std::unique_ptr<SqliteDatabase>     m_db;
    std::unique_ptr<SnippetRepository>  m_snippetRepo;
    std::unique_ptr<SolutionRepository> m_solutionRepo;
    std::unique_ptr<SearchIndex>        m_searchIndex;

    Sidebar        *m_sidebar = nullptr;
    QStackedWidget *m_stack   = nullptr;

    enum PageIndex { Snippets = 0, Solutions = 1, Search = 2,
                     StressTest = 3, Workspaces = 4, Templates = 5 };

    QWidget           *m_snippetsPage  = nullptr;
    SnippetListView   *m_snippetList   = nullptr;
    SnippetEditorView *m_snippetEditor = nullptr;

    QWidget            *m_solutionsPage  = nullptr;
    SolutionListView   *m_solutionList   = nullptr;
    SolutionEditorView *m_solutionEditor = nullptr;

    SearchView     *m_searchView     = nullptr;
    StressTestView *m_stressTestView = nullptr;
    WorkspaceView  *m_workspaceView  = nullptr;
    TemplateView   *m_templateView   = nullptr;
};