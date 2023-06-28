#include "textindentdelegate.h"

#include <QApplication>
#include <QPainter>

void TextIndentDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const {

    QStyleOptionViewItem newOption(option);
    initStyleOption(&newOption, index);

    // Temporarily clear the text to prevent it from being drawn by drawControl
    QString originalText = newOption.text;
    newOption.text.clear();

    // Draw the background and focus rectangle
    newOption.state.setFlag(QStyle::State_HasFocus, false);
    QApplication::style()->drawControl(QStyle::CE_ItemViewItem, &newOption, painter);

    // Restore the original text and draw it using QPainter with indentation
    newOption.text = originalText;
    painter->save();
    painter->setPen(newOption.palette.text().color());
    painter->setFont(newOption.font);
    QRect textRect = newOption.rect;
    textRect.setLeft(textRect.left() + m_indent);
    painter->drawText(textRect, newOption.displayAlignment, newOption.text);
    painter->restore();
}
