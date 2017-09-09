#ifndef PRINTUTILS_H
#define PRINTUTILS_H

#include <QtSerialPort/QtSerialPort>
#include "note.h"
#include "reminder.h"
#include "item.h"
#include "consumption.h"

namespace Printer
{
  bool init(QSerialPort& printer,
            QString& error);

  bool print(QSerialPort& printer,
             const QString& msg,
             QString& error);
}

namespace NotePrinter
{
  QString build(const Note& note);
}

namespace ReminderPrinter
{
  QString build(const Reminder& r);
}

namespace ConsumptionPrinter
{
  QString build(qint64 date,
                const QVector<Consumption>& consumptions,
                const QVector<Item>& items,
                double total);

  QString build(const QVector<qint64>& dates,
                const QVector<double>& totals,
                double total);
}
#endif // PRINTUTILS_H
