#ifndef RESERVATION_H
#define RESERVATION_H

#include <QObject>
#include <QString>
#include <QVector>
#include <QDateTime>
#include "defines.h"
#include "jitem.h"

struct Reservation : public JItem
{
  qlonglong m_number;
  QString m_name;
  QString m_location;
  QDateTime m_dateTime;
  int m_ammount;
  QString m_observation;
  qlonglong m_employeeId;

  void clear()
  {
    m_number = 0;
    m_dateTime = QDateTime::currentDateTime();
    m_location.clear();
    m_name.clear();
    m_ammount = 0;
    m_observation.clear();
    m_employeeId = INVALID_ID;
  }

  Reservation()
  {
    clear();
  }

  bool operator != (const JItem& other) const
  {
    const Reservation& another = dynamic_cast<const Reservation&>(other);
    return m_dateTime != another.m_dateTime ||
           m_location != another.m_location ||
           m_name != another.m_name ||
           m_ammount != another.m_ammount ||
           m_number != another.m_number;
  }

  bool operator == (const JItem& other) const
  {
    return !(*this != other);
  }

  bool isValid() const
  {
    return false;
  }
};

#endif // RESERVATION_H