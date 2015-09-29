/*
 * Copyright (C) Kreogist Dev Team
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software Foundation,
 * Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 */

#ifndef KNMUSICCATEGORYMODEL_H
#define KNMUSICCATEGORYMODEL_H

#include <QList>

#include "knmusicutil.h"

#include <QAbstractListModel>

using namespace MusicUtil;

class KNMusicCategoryModel : public QAbstractListModel
{
    Q_OBJECT
public:
    enum CategoryRole
    {
        CategorySize=Qt::UserRole
    };

    explicit KNMusicCategoryModel(QObject *parent = 0);

    /*!
     * \brief Reset the category model. This will add the no category item to
     * the model.
     */
    void reset();

    QModelIndex categoryIndex(const QVariant &categoryText);

    /*!
     * \brief Reimplemented from QAbstractListModel::rowCount().
     */
    int rowCount(const QModelIndex &parent=QModelIndex()) const Q_DECL_OVERRIDE;

    /*!
     * \brief Reimplemented from QAbstractListModel::data().
     */
    QVariant data(const QModelIndex &index, int role) const Q_DECL_OVERRIDE;

    /*!
     * \brief Reimplemented from QAbstractListModel::flags().
     */
    Qt::ItemFlags flags(const QModelIndex &index) const Q_DECL_OVERRIDE;

    /*!
     * \brief Reimplemented from QAbstractListModel::headerData().
     */
    QVariant headerData(int section,
                        Qt::Orientation orientation,
                        int role = Qt::DisplayRole) const Q_DECL_OVERRIDE;

    /*!
     * \brief Get the no category text.
     * \return The no category text.
     */
    QString noCategoryText() const;

    /*!
     * \brief Reimplemented from QAbstractListModel::setData().
     */
    bool setData(const QModelIndex &index,
                 const QVariant &value,
                 int role) Q_DECL_OVERRIDE;

    /*!
     * \brief Get the category column.
     * \return The category column number.
     */
    int categoryColumn() const;

signals:

public slots:
    /*!
     * \brief Set the category column number, it will be used to category the
     * detail info.
     * \param categoryColumn The category column.
     */
    void setCategoryColumn(int categoryColumn);

    /*!
     * \brief Set the no album art icon.
     * \param noAlbumArt The no album art icon.
     */
    void setNoAlbumArt(const QPixmap &noAlbumArt);

    /*!
     * \brief Set no category text of the first data.
     * \param noCategoryText
     */
    void setNoCategoryText(const QString &noCategoryText);

    void onCategoryAdd(const KNMusicDetailInfo &detailInfo);

    void onCategoryRemove(const KNMusicDetailInfo &detailInfo);

private:
    struct CategoryItem
    {
        QVariant displayText;
        QStringList albumArtHash;
        int count;
        CategoryItem() :
            displayText(QVariant()),
            albumArtHash(QStringList()),
            count(0)
        {
        }
        inline bool operator == (const CategoryItem &value)
        {
            return displayText==value.displayText;
        }
    };

    inline CategoryItem generateNoCategoryItem();
    inline void appendItem(const CategoryItem &item);
    inline void removeItem(const int &row);
    inline void replaceItem(const int &row, const CategoryItem &item);
    inline void reduceCount(const int &row);

    QList<CategoryItem> m_categoryList;
    QVariant m_noAlbumArt;
    QString m_noCategoryText;
    int m_categoryColumn;
};

#endif // KNMUSICCATEGORYMODEL_H