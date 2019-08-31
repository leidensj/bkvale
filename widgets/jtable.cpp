#include "jtable.h"
#include <QTableWidget>
#include <QKeyEvent>
#include <QLayout>
#include <QPushButton>
#include "tinyexpr.h"
#include "jregexpvalidator.h"

JTable::JTable(int flags, QWidget* parent)
 : QTableWidget(parent)
{
  if (flags & (int)Flags::BigFont)
  {
    QFont f = font();
    f.setPointSize(12);
    setFont(f);
    f = horizontalHeader()->font();
    f.setPointSize(12);
  }

  if (flags & (int)Flags::Uppercase)
  {
    QFont f = font();
    f.setCapitalization(QFont::AllUppercase);
    setFont(f);
    f.setCapitalization(QFont::Capitalize);
    horizontalHeader()->setFont(f);
  }

  setSelectionBehavior(QAbstractItemView::SelectItems);
  setSelectionMode(QAbstractItemView::SingleSelection);
  horizontalHeader()->setHighlightSections(false);
  verticalHeader()->setSectionsMovable(true);
  verticalHeader()->setHighlightSections(false);

  QObject::connect(this, SIGNAL(currentCellChanged(int, int, int, int)), this, SLOT(emitChangedSignal()));
  QObject::connect(this, SIGNAL(cellDoubleClicked(int, int)), this, SLOT(emitActivateSignal(int, int)));
}

void JTable::removeItem()
{
  if (isValidRow(currentRow()))
    removeRow(currentRow());
  emitChangedSignal();
}

void JTable::removeAllItems()
{
  if (hasItems())
  {
    setRowCount(0);
    emitChangedSignal();
  }
}

bool JTable::hasItems() const
{
  return rowCount() != 0;
}

void JTable::keyPressEvent(QKeyEvent *event)
{
  if (event->key() == Qt::Key_Enter || event->key() == Qt::Key_Return)
  {
    QKeyEvent modEvent(event->type(),
                       Qt::Key_Tab,
                       event->modifiers(),
                       event->nativeScanCode(),
                       event->nativeVirtualKey(),
                       event->nativeModifiers(),
                       event->text(),
                       event->isAutoRepeat(),
                       event->count());
    QTableWidget::keyPressEvent(&modEvent);
  }
  else if (event->key() == Qt::Key_Delete)
  {
    if (currentIndex().isValid())
      emit deleteSignal(currentIndex().row(), currentIndex().column());
    QTableWidget::keyPressEvent(event);
  }
  else if (event->key() == Qt::Key_Space)
  {
    if (currentIndex().isValid())
      emit activateSignal(currentIndex().row(), currentIndex().column());
    QTableWidget::keyPressEvent(event);
  }
  else
  {
    QTableWidget::keyPressEvent(event);
  }

}

void JTable::emitChangedSignal()
{
  emit changedSignal(isValidCurrentRow());
}


void JTable::emitDeleteSignal(int row, int column)
{
  emit deleteSignal(row, column);
}

void JTable::emitActivateSignal(int row, int column)
{
  emit activateSignal(row, column);
}

bool JTable::isValidRow(int row) const
{
  return row >= 0 && row < rowCount();
}

bool JTable::isValidCurrentRow() const
{
  return isValidRow(currentRow());
}

JItemTable::JItemTable(int flags, QWidget* parent)
  : JTable(flags, parent)
{
  connect(this, SIGNAL(cellChanged(int, int)), this, SLOT(update(int, int)));
  connect(this, SIGNAL(activateSignal(int,int)), this, SLOT(itemActivate(int, int)));
  connect(this, SIGNAL(deleteSignal(int,int)), this, SLOT(itemDelete(int, int)));
}

void JItemTable::setHeaderIcon(int pos, const QIcon& icon)
{
  model()->setHeaderData(pos,
                         Qt::Horizontal,
                         icon,
                         Qt::DecorationRole);
}

void JItemTable::setHeaderIconSearchable(int pos)
{
  setHeaderIcon(pos, QIcon(":/icons/res/binoculars.png"));
}

DoubleItem::DoubleItem(Data::Type type,
                       Color color,
                       bool bCheckable,
                       bool bAcceptNegative,
                       const QString& prefix,
                       const QString& sufix)
  : m_type(type)
  , m_color(color)
  , m_bCheckable(bCheckable)
  , m_bAcceptNegative(bAcceptNegative)
  , m_prefix(prefix)
  , m_sufix(sufix)
{
  if (m_bCheckable)
  {
    setFlags(flags() | Qt::ItemIsUserCheckable);
    setCheckState(Qt::Checked);
  }
}

void DoubleItem::setValue(double val)
{
  setData(Qt::UserRole, val);
  setText(m_prefix + Data::str(val, m_type) + m_sufix);

  if (m_bCheckable)
  {
    if (checkState() == Qt::Unchecked)
      setFlags(flags() & ~Qt::ItemIsEditable);
    else
      setFlags(flags() | Qt::ItemIsEditable);
  }

  switch (m_color)
  {
    case Color::Background:
      setBackgroundColor(val == 0.0 ? QColor(255, 200, 200) : QColor(Qt::white));
      break;
    case Color::Foreground:
      setTextColor(QColor(val >= 0 ? Qt::red : Qt::darkGreen));
      break;
    case Color::None:
    default:
      break;
  }
}

double DoubleItem::getValue() const
{
  return data(Qt::UserRole).toDouble();
}

bool DoubleItem::evaluate(const QString& exp)
{
  auto stdExp = exp.toStdString();
  int error = 0;
  double val = te_interp(stdExp.c_str(), &error);
  bool bValid = !error;
  if (bValid)
  {
    if (!m_bAcceptNegative && val < 0)
      bValid = false;
    else
      setValue(val);
  }
  return bValid;
}

void DoubleItem::evaluate()
{
  if (!evaluate(text()))
    setValue(getValue());
}

DateItem::DateItem(const QDate& defaultDate, Color color)
  : m_defaultDate(defaultDate)
  , m_color(color)
{
  setDate(m_defaultDate);
}

void DateItem::setDate(const QDate& dt)
{
  setData(Qt::UserRole, dt);
  setText(dt.toString("dd/MM/yyyy"));

  switch (m_color)
  {
    case Color::DateBeforeDefault:
      setBackgroundColor(dt < m_defaultDate ? QColor(255, 200, 200) : QColor(Qt::white));
      setToolTip(dt < m_defaultDate ? "A data é anterior a " + m_defaultDate.toString("dd/MM/yyyy") : "");
      break;
    case Color::None:
    default:
      break;
  }
}

QDate DateItem::getDate()const
{
  return data(Qt::UserRole).toDate();
}

bool DateItem::evaluate(const QString& exp)
{
  QDate dt = QDate::fromString(exp, "dd/MM/yyyy");
  if (!dt.isValid())
  {
    dt = QDate::fromString(exp, "dd/MM/yy");
    if (!dt.isValid())
    {
      dt = QDate::fromString(exp, "ddMMyyyy");
      if (!dt.isValid())
      {
        dt = QDate::fromString(exp, "ddMM");
        dt.setDate(QDate::currentDate().year(), dt.month(), dt.day());
        if (!dt.isValid())
        {
          dt = QDate::fromString(exp, "dd");
          dt.setDate(QDate::currentDate().year(), QDate::currentDate().month(), dt.day());
        }
      }
    }
  }

  if (dt.isValid())
    setDate(dt);

  return dt.isValid();
}

void DateItem::evaluate()
{
  if (!evaluate(text()))
    setDate(getDate());
}

TimeItem::TimeItem(const QTime& defaultTime)
  : m_defaultTime(defaultTime)
{
  setTime(m_defaultTime);
}

void TimeItem::setTime(const QTime& t)
{
  setData(Qt::UserRole, t);
  setText(t.toString("HH:mm"));
}

QTime TimeItem::getTime()const
{
  return data(Qt::UserRole).toTime();
}

bool TimeItem::evaluate(const QString& exp)
{
  QTime t = QTime::fromString(exp, "HH:mm");
  if (!t.isValid())
    t = QTime::fromString(exp, "HHmm");

  if (t.isValid())
    setTime(t);

  return t.isValid();
}

void TimeItem::evaluate()
{
  if (!evaluate(text()))
    setTime(getTime());
}

TextItem::TextItem(Text::Input input, bool toUpper)
  : m_input(input)
  , m_toUpper(toUpper)
{

}

bool TextItem::evaluate(const QString& exp)
{
  JRegExpValidator val(m_toUpper, QRegExp(Text::getRegEx(m_input)));
  QString exp2 = exp;
  if (m_toUpper)
    exp2.toUpper();
  int pos = exp2.length();
  bool bValid = val.validate(exp2, pos) == QValidator::State::Acceptable;
  if (bValid)
  {
    setData(Qt::UserRole, exp2);
    setText(exp2);
  }
  return bValid;
}

void TextItem::evaluate()
{
  if (!evaluate(text()))
    setText(data(Qt::UserRole).toString());
}
