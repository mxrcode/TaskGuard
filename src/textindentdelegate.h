#ifndef TEXTINDENTDELEGATE_H
#define TEXTINDENTDELEGATE_H

#include <QStyledItemDelegate>

class TextIndentDelegate : public QStyledItemDelegate {
    Q_OBJECT

public:
    TextIndentDelegate(int indent, QObject *parent = nullptr)
        : QStyledItemDelegate(parent), m_indent(indent) {}

    void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const override;

private:
    int m_indent;
};

#endif // TEXTINDENTDELEGATE_H
