#include "PlotWidget.h"
#include "PlotData.h"
#include "Plot.h"
#include "EditableTabWidget.h"
#include <QtGui/QPainter>
#include <QtGui/QKeyEvent>

PlotWidget::PlotWidget(std::shared_ptr<Plot> plot, QWidget* parent) 
{
	leftWidth_ = -1;
	plot_ = plot;
	layout_ = new QHBoxLayout();
	setLayout(layout_);
	splitter_ = new QSplitter(Qt::Horizontal);
	layout_->addWidget(splitter_);

	node_list_ = new GraphNodeList(plot_, splitter_);
	tabs_ = new EditableTabWidget();
	tabs_->setTabShape(QTabWidget::Triangular);
	(void)connect(tabs_->tabBar(), &QTabBar::tabBarDoubleClicked, this, &PlotWidget::editTabText);
	(void)connect(tabs_, &QTabWidget::tabCloseRequested, this, &PlotWidget::closeTab);

	splitter_->addWidget(node_list_);
	splitter_->addWidget(tabs_);

	auto plotview = new GraphWidget(plot_, splitter_);
	tabs_->addTab(plotview, "1");


	editor_ = nullptr;

	connect(plot.get(), &Plot::dataArrived, this, &PlotWidget::dataArrived);
	connect(plot.get(), &Plot::restarted, this, &PlotWidget::restarted);
	connect(plot.get(), &Plot::columnsDefined, this, &PlotWidget::columnsDefined);
	connect(plot.get(), &Plot::complete, this, &PlotWidget::complete);
}


void PlotWidget::columnsDefined(QStringList columns)
{
	node_list_->columnsDefined(columns);
}

void PlotWidget::restarted(const QString& name)
{
	node_list_->restarted();

	for (int i = 0; i < tabs_->count(); i++) 
	{
		GraphWidget* wid = dynamic_cast<GraphWidget*>(tabs_->widget(i));
		if (wid != nullptr) {
			wid->restart();
		}
	}
}

void PlotWidget::dataArrived(const QString& name)
{
	node_list_->dataArrived(name);

	for (int i = 0; i < tabs_->count(); i++)
	{
		GraphWidget* wid = dynamic_cast<GraphWidget*>(tabs_->widget(i));
		if (wid != nullptr) {
			wid->updateAllData();
		}
	}
}

void PlotWidget::complete(const QString& name)
{

}

void PlotWidget::closeTab(int index)
{
	if (index == -1)
		index = tabs_->currentIndex();

	auto* gr = tabs_->widget(index);
	tabs_->removeTab(index);
	delete gr;
}

void PlotWidget::editTabText(int which)
{
	which_tab_ = which;

	QTabBar* bar = tabs_->tabBar();

	QRect r = bar->tabRect(which);

	if (editor_ == nullptr)
	{
		editor_ = new TabEditName(bar);
		(void)connect(editor_, &TabEditName::returnPressed, this, &PlotWidget::editTabDone);
		(void)connect(editor_, &TabEditName::escapePressed, this, &PlotWidget::editTabAborted);
	}

	editor_->setGeometry(r);
	editor_->setFocus(Qt::FocusReason::OtherFocusReason);
	editor_->setVisible(true);
	editor_->setText(bar->tabText(which));
	editor_->selectAll();
}

void PlotWidget::editTabDone()
{
	QTabBar* bar = tabs_->tabBar();
	QString txt = editor_->text();

	editor_->setVisible(false);
	bar->setTabText(which_tab_, txt);
}

void PlotWidget::editTabAborted()
{
	editor_->setVisible(false);
}

void PlotWidget::paintEvent(QPaintEvent* ev)
{
	QPainter p(this);
	QBrush b(Qt::GlobalColor::darkGray);
	p.setBrush(b);
	p.drawRect(0, 0, width(), height());
}

void PlotWidget::splitterMoved(int pos, int index)
{
	leftWidth_ = pos;
}

void PlotWidget::keyPressEvent(QKeyEvent* ev)
{
	if (ev->key() == Qt::Key_Insert) {
		QString name = QString::number(tabs_->count() + 1);
		auto plotview = new GraphWidget(plot_, splitter_);
		tabs_->addTab(plotview, name);
	}
}

void PlotWidget::resizeEvent(QResizeEvent* ev)
{
	QList<int> list;
	int leftv = leftWidth_;

	if (leftv == -1) {
		leftv = 120;
	}

	list.push_back(leftv);
	list.push_back(width() - leftv);
	splitter_->setSizes(list);
}