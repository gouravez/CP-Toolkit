#include "mainwindow.h"
#include "sidebar.h"

#include "snippets/snippetlistview.h"
#include "snippets/snippeteditorview.h"
#include "solutions/solutionlistview.h"
#include "solutions/solutioneditorview.h"
#include "search/searchview.h"
#include "stresstest/stresstestview.h"
#include "workspace/workspaceview.h"
#include "templates/templateview.h"

#include "../dal/sqlitedatabase.h"
#include "../dal/snippetrepository.h"
#include "../dal/solutionrepository.h"

#include "../core/searchindex.h"

#include "../models.h"

#include <QStackedWidget>
#include <QHBoxLayout>
#include <QWidget>
#include <QShortcut>
#include <QKeySequence>
#include <QStandardPaths>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    setWindowTitle("Competitive Programming Toolkit");
    setMinimumSize(1100, 700);
    resize(1280, 800);

    setupDatabase();
    setupLayout();
    setupShortcuts();

    refreshSnippets();
    refreshSolutions();
    buildSearchIndex();

    connect(m_sidebar, &Sidebar::itemSelected,
            this,      &MainWindow::onSidebarItemSelected);
}

MainWindow::~MainWindow() {}

void MainWindow::setupDatabase()
{
    const QString dataDir = QStandardPaths::writableLocation(
        QStandardPaths::AppDataLocation);
    const QString dbPath  = dataDir + "/cpt.db";

    m_db           = std::make_unique<SqliteDatabase>(dbPath);
    m_db->open();

    m_snippetRepo  = std::make_unique<SnippetRepository>(m_db.get());
    m_solutionRepo = std::make_unique<SolutionRepository>(m_db.get());
    m_searchIndex  = std::make_unique<SearchIndex>();

    m_snippetRepo->initialize();
    m_solutionRepo->initialize();
}

void MainWindow::setupLayout()
{
    QWidget *central = new QWidget(this);
    setCentralWidget(central);

    QHBoxLayout *layout = new QHBoxLayout(central);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(0);

    m_sidebar = new Sidebar(this);
    m_stack   = new QStackedWidget(this);

    // Page 0: Snippets
    m_snippetsPage = new QWidget(this);
    {
        QHBoxLayout *pageLayout = new QHBoxLayout(m_snippetsPage);
        pageLayout->setContentsMargins(0, 0, 0, 0);
        pageLayout->setSpacing(0);

        m_snippetList   = new SnippetListView(this);
        m_snippetEditor = new SnippetEditorView(this);

        pageLayout->addWidget(m_snippetList);
        pageLayout->addWidget(m_snippetEditor, 1);
    }
    connect(m_snippetList,   &SnippetListView::snippetSelected,
            this,            &MainWindow::onSnippetSelected);
    connect(m_snippetList,   &SnippetListView::createRequested,
            this,            &MainWindow::onSnippetCreateRequested);
    connect(m_snippetList,   &SnippetListView::deleteRequested,
            this,            &MainWindow::onSnippetDeleteRequested);
    connect(m_snippetEditor, &SnippetEditorView::saved,
            this,            &MainWindow::onSnippetSaved);

    // Page 1: Solutions
    m_solutionsPage = new QWidget(this);
    {
        QHBoxLayout *pageLayout = new QHBoxLayout(m_solutionsPage);
        pageLayout->setContentsMargins(0, 0, 0, 0);
        pageLayout->setSpacing(0);

        m_solutionList   = new SolutionListView(this);
        m_solutionEditor = new SolutionEditorView(this);

        pageLayout->addWidget(m_solutionList);
        pageLayout->addWidget(m_solutionEditor, 1);
    }
    connect(m_solutionList,   &SolutionListView::solutionSelected,
            this,             &MainWindow::onSolutionSelected);
    connect(m_solutionList,   &SolutionListView::createRequested,
            this,             &MainWindow::onSolutionCreateRequested);
    connect(m_solutionList,   &SolutionListView::deleteRequested,
            this,             &MainWindow::onSolutionDeleteRequested);
    connect(m_solutionEditor, &SolutionEditorView::saved,
            this,             &MainWindow::onSolutionSaved);

    // Page 2: Search
    m_searchView = new SearchView(this);
    connect(m_searchView, &SearchView::snippetActivated,
            this,         &MainWindow::onSnippetActivated);
    connect(m_searchView, &SearchView::solutionActivated,
            this,         &MainWindow::onSolutionActivated);

    // Page 3: Stress Test
    m_stressTestView = new StressTestView(this);

    // Page 4: Workspaces
    m_workspaceView = new WorkspaceView(this);

    // Page 5: Templates
    m_templateView = new TemplateView(this);

    m_stack->addWidget(m_snippetsPage);
    m_stack->addWidget(m_solutionsPage);
    m_stack->addWidget(m_searchView);
    m_stack->addWidget(m_stressTestView);
    m_stack->addWidget(m_workspaceView);
    m_stack->addWidget(m_templateView);

    layout->addWidget(m_sidebar);
    layout->addWidget(m_stack, 1);
}

void MainWindow::setupShortcuts()
{
    QShortcut *searchShortcut = new QShortcut(QKeySequence("Ctrl+K"), this);
    connect(searchShortcut, &QShortcut::activated,
            this,           &MainWindow::onGlobalSearch);
}

void MainWindow::buildSearchIndex()
{
    m_searchIndex->build(m_snippetRepo->getAll(), m_solutionRepo->getAll());
    m_searchView->setSearchIndex(m_searchIndex.get());
}

void MainWindow::refreshSnippets()
{
    const QList<Snippet> snippets   = m_snippetRepo->getAll();
    const QList<QString> categories = m_snippetRepo->getCategories();

    m_snippetList->setSnippets(snippets);
    m_snippetList->setCategories(categories);
    m_snippetEditor->setKnownCategories(categories);

    m_templateView->setSnippets(snippets);
}

void MainWindow::refreshSolutions()
{
    const QList<Solution> solutions = m_solutionRepo->getAll();
    const QList<QString>  platforms = m_solutionRepo->getPlatforms();

    m_solutionList->setSolutions(solutions);
    m_solutionList->setPlatforms(platforms);
    m_solutionEditor->setKnownPlatforms(platforms);
}

void MainWindow::onSidebarItemSelected(int index)
{
    if (index >= 0 && index < m_stack->count())
        m_stack->setCurrentIndex(index);
}

void MainWindow::onSnippetSelected(int id)
{
    const Snippet s = m_snippetRepo->getById(id);
    if (s.id != -1)
        m_snippetEditor->loadSnippet(s);
}

void MainWindow::onSnippetCreateRequested()
{
    m_snippetEditor->setCreateMode();
}

void MainWindow::onSnippetDeleteRequested(int id)
{
    if (m_snippetRepo->remove(id)) {
        m_searchIndex->removeSnippet(id);
        m_searchView->refresh();
        refreshSnippets();
    }
}

void MainWindow::onSnippetSaved(const Snippet &snippet)
{
    if (snippet.id == -1) {
        const int newId = m_snippetRepo->create(snippet);
        if (newId != -1) {
            Snippet saved = snippet;
            saved.id = newId;
            m_searchIndex->upsert(saved);
        }
    } else {
        m_snippetRepo->update(snippet);
        m_searchIndex->upsert(snippet);
    }
    m_searchView->refresh();
    refreshSnippets();
}

void MainWindow::onSolutionSelected(int id)
{
    const Solution s = m_solutionRepo->getById(id);
    if (s.id != -1)
        m_solutionEditor->loadSolution(s);
}

void MainWindow::onSolutionCreateRequested()
{
    m_solutionEditor->setCreateMode();
}

void MainWindow::onSolutionDeleteRequested(int id)
{
    if (m_solutionRepo->remove(id)) {
        m_searchIndex->removeSolution(id);
        m_searchView->refresh();
        refreshSolutions();
    }
}

void MainWindow::onSolutionSaved(const Solution &solution)
{
    if (solution.id == -1) {
        const int newId = m_solutionRepo->create(solution);
        if (newId != -1) {
            Solution saved = solution;
            saved.id = newId;
            m_searchIndex->upsert(saved);
        }
    } else {
        m_solutionRepo->update(solution);
        m_searchIndex->upsert(solution);
    }
    m_searchView->refresh();
    refreshSolutions();
}

void MainWindow::onSnippetActivated(int id)
{
    m_stack->setCurrentIndex(PageIndex::Snippets);
    m_sidebar->selectButton(PageIndex::Snippets);
    onSnippetSelected(id);
}

void MainWindow::onSolutionActivated(int id)
{
    m_stack->setCurrentIndex(PageIndex::Solutions);
    m_sidebar->selectButton(PageIndex::Solutions);
    onSolutionSelected(id);
}

void MainWindow::onGlobalSearch()
{
    m_stack->setCurrentIndex(PageIndex::Search);
    m_searchView->focusSearchBar();
}