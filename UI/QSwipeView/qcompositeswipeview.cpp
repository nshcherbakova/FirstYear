#include "qcompositeswipeview.hpp"
#include "qcompositeswipeview_priv.hpp"

QCompositeSwipeView::QCompositeSwipeView(QWidget *parent) : QWidget(parent) {
  ui = new QCompositeSwipeView_Priv(this);
  ui->pageIndicator->setEnabled(false);
  connect(ui->swipeView->stacked_widget_, &QStackedWidget::currentChanged,
          [this] {
            ui->pageIndicator->setPageCount(
                ui->swipeView->stacked_widget_->count());
            ui->pageIndicator->setCurrentPage(
                ui->swipeView->stacked_widget_->currentIndex());
            emit currentChanged(ui->swipeView->stacked_widget_->currentIndex());
          });
  connect(ui->swipeView->stacked_widget_, &QStackedWidget::widgetRemoved,
          [this](int index) {
            ui->pageIndicator->setPageCount(
                ui->swipeView->stacked_widget_->count());
            ui->pageIndicator->setCurrentPage(
                ui->swipeView->stacked_widget_->currentIndex());
            emit widgetRemoved(index);
          });
}

QCompositeSwipeView::~QCompositeSwipeView() { delete ui; }

int QCompositeSwipeView::addWidget(QWidget *child) {
  const int rval = ui->swipeView->stacked_widget_->addWidget(child);
  ui->pageIndicator->setPageCount(ui->swipeView->stacked_widget_->count());
  return rval;
}

int QCompositeSwipeView::insertWidget(int index, QWidget *w) {
  const int rval = ui->swipeView->stacked_widget_->insertWidget(index, w);
  ui->pageIndicator->setPageCount(ui->swipeView->stacked_widget_->count());
  return rval;
}

void QCompositeSwipeView::removeWidget(QWidget *w) {
  ui->swipeView->stacked_widget_->removeWidget(w);
  ui->pageIndicator->setPageCount(ui->swipeView->stacked_widget_->count());
}

QWidget *QCompositeSwipeView::currentWidget() const {
  return ui->swipeView->stacked_widget_->currentWidget();
}

int QCompositeSwipeView::currentIndex() const {
  return ui->swipeView->stacked_widget_->currentIndex();
}

int QCompositeSwipeView::indexOf(const QWidget *w) const {
  return ui->swipeView->stacked_widget_->indexOf(w);
}

QWidget *QCompositeSwipeView::widget(int index) const {
  return ui->swipeView->stacked_widget_->widget(index);
}

int QCompositeSwipeView::count() const {
  return ui->swipeView->stacked_widget_->count();
}

void QCompositeSwipeView::setCurrentIndex(int index) {
  ui->swipeView->stacked_widget_->setCurrentIndex(index);
}

void QCompositeSwipeView::setCurrentWidget(QWidget *w) {
  ui->swipeView->stacked_widget_->setCurrentWidget(w);
}

int QCompositeSwipeView::animationSpeed() const {
  return ui->swipeView->animationSpeed();
}

void QCompositeSwipeView::animationSpeed(int animationSpeed_) {
  ui->swipeView->animationSpeed(animationSpeed_);
}

int QCompositeSwipeView::swipeVelocity() const {
  return ui->swipeView->swipeVelocity();
}

void QCompositeSwipeView::swipeVelocity(int swipeVelocity_) {
  ui->swipeView->swipeVelocity(swipeVelocity_);
}

bool QCompositeSwipeView::swipeVertical() const {
  return ui->swipeView->swipeVertical();
}

void QCompositeSwipeView::swipeVertical(bool swipeVertical_) {
  ui->swipeView->swipeVertical(swipeVertical_);
}
