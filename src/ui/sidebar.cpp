#include "sidebar.h"

#include <QPushButton>
#include <QVBoxLayout>
#include <QLabel>

Sidebar::Sidebar(QWidget *parent)
    : QWidget(parent)
{
    setFixedWidth(180);
    setObjectName("Sidebar");
    setupUI();
}

void Sidebar::setupUI()
{
    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(0);

    QLabel *title = new QLabel("CPT", this);
    title->setObjectName("SidebarTitle");
    title->setAlignment(Qt::AlignCenter);
    title->setFixedHeight(60);
    layout->addWidget(title);

    QLabel *libraryLabel = new QLabel("LIBRARY", this);
    libraryLabel->setObjectName("SidebarGroupLabel");
    libraryLabel->setContentsMargins(16, 12, 0, 4);
    layout->addWidget(libraryLabel);

    m_snippetsBtn  = createButton("Snippets",  0);
    m_solutionsBtn = createButton("Solutions", 1);
    m_searchBtn    = createButton("Search",    2);

    layout->addWidget(m_snippetsBtn);
    layout->addWidget(m_solutionsBtn);
    layout->addWidget(m_searchBtn);

    QLabel *toolsLabel = new QLabel("TOOLS", this);
    toolsLabel->setObjectName("SidebarGroupLabel");
    toolsLabel->setContentsMargins(16, 16, 0, 4);
    layout->addWidget(toolsLabel);

    m_stressTestBtn = createButton("Stress Test", 3);
    m_workspacesBtn = createButton("Workspaces",  4);
    m_templatesBtn  = createButton("Templates",   5);

    layout->addWidget(m_stressTestBtn);
    layout->addWidget(m_workspacesBtn);
    layout->addWidget(m_templatesBtn);

    layout->addStretch();

    onButtonClicked(0);
}

QPushButton *Sidebar::createButton(const QString &text, int index)
{
    QPushButton *btn = new QPushButton(text, this);
    btn->setObjectName("SidebarButton");
    btn->setFixedHeight(40);
    btn->setCheckable(true);
    btn->setCursor(Qt::PointingHandCursor);

    connect(btn, &QPushButton::clicked, this, [this, index]() {
        onButtonClicked(index);
    });

    return btn;
}

void Sidebar::onButtonClicked(int index)
{
    if (m_activeButton)
        m_activeButton->setChecked(false);

    QPushButton *buttons[] = {
        m_snippetsBtn, m_solutionsBtn, m_searchBtn,
        m_stressTestBtn, m_workspacesBtn, m_templatesBtn
    };

    m_activeButton = buttons[index];
    m_activeButton->setChecked(true);

    emit itemSelected(index);
}

void Sidebar::selectButton(int index)
{
    if (m_activeButton)
        m_activeButton->setChecked(false);

    QPushButton *buttons[] = {
        m_snippetsBtn, m_solutionsBtn, m_searchBtn,
        m_stressTestBtn, m_workspacesBtn, m_templatesBtn
    };

    if (index < 0 || index >= 6) return;
    m_activeButton = buttons[index];
    m_activeButton->setChecked(true);
}