#ifndef MODELITEMSORT_H
#define MODELITEMSORT_H

#include <QSortFilterProxyModel>

class ModelItemSort : public QSortFilterProxyModel
{
    Q_OBJECT
public:
    ModelItemSort(QObject* parent = nullptr);

protected:
    bool lessThan(const QModelIndex& sLeft, const QModelIndex& sRight) const;
};

#endif // MODELITEMSORT_H
