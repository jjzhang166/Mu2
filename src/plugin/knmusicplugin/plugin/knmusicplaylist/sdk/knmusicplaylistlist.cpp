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
#include <QBoxLayout>

#include "knlinearsensewidget.h"
#include "knanimationmenu.h"
#include "knopacityanimebutton.h"
#include "knthememanager.h"
#include "knlocalemanager.h"
#include "kndpimanager.h"

#include "knmusicplaylistlistview.h"
#include "knmusicplaylistlistmodel.h"

#include "knmusicplaylistlist.h"

KNMusicPlaylistList::KNMusicPlaylistList(QWidget *parent) :
    QWidget(parent),
    m_playlistListView(new KNMusicPlaylistListView(this)),
    m_playlistListEditor(new KNLinearSenseWidget(this))
{
    //Configure the editor.
    configureEditor();

    //Initial the main layout.
    QBoxLayout *mainLayout=new QBoxLayout(QBoxLayout::TopToBottom,
                                          this);
    mainLayout->setContentsMargins(0,0,0,0);
    mainLayout->setSpacing(0);
    setLayout(mainLayout);
    //Add widget to main layout.
    mainLayout->addWidget(m_playlistListView, 1);
    mainLayout->addWidget(m_playlistListEditor);

    //Link to retranslate.
    knI18n->link(this, &KNMusicPlaylistList::retranslate);
    retranslate();
}

void KNMusicPlaylistList::setPlaylistList(KNMusicPlaylistListModel *model)
{
    //Set the model to the playlist list view.
    m_playlistListView->setModel(model);
    //After set the model, the selection model will be update.
    //Capture the currentChanged() signal from the list view, link to the
    //requireShowPlaylist() signal.
    connect(m_playlistListView->selectionModel(),
            &QItemSelectionModel::currentChanged,
            [=](const QModelIndex &current)
            {
                //Simply emit the signal is alright.
                emit requireShowPlaylist(current);
            });
}

void KNMusicPlaylistList::renamePlaylist(const QModelIndex &index)
{
    //Ask the playlist list view to rename the playlist.
    m_playlistListView->edit(index);
}

QModelIndex KNMusicPlaylistList::currentIndex() const
{
    return m_playlistListView->currentIndex();
}

void KNMusicPlaylistList::showPlaylist(const QModelIndex &index)
{
    //Set the current index to the prefer index.
    m_playlistListView->setCurrentIndex(index);
}

void KNMusicPlaylistList::retranslate()
{
    //Update the actions translation.
    //Add Menu
    m_addActions[AddPlaylist]->setText(tr("New Playlist"));
    m_addActions[ImportPlaylist]->setText(tr("Import Playlist"));
    //Configure Menu
    m_configureAction[ExportPlaylist]->setText(tr("Export"));
    m_configureAction[CopyPlaylist]->setText(tr("Copy"));
}

KNAnimationMenu *KNMusicPlaylistList::generateEditorMenu()
{
    //Generate the menu.
    KNAnimationMenu *menu=new KNAnimationMenu(this);
    //Set the object name.
    menu->setObjectName("PlaylistListEditorMenu");
    //Set the seperator style sheet.
    menu->setStyleSheet("QMenu::separator {height:1px;"
                        "background: rgba(255,255,255,100);"
                        "margin-left: 5px;"
                        "margin-right: 5px;}");
    //Add menu to theme manager.
    knTheme->registerWidget(menu);
    //Give back the menu.
    return menu;
}

inline void KNMusicPlaylistList::configureEditor()
{
    //Set the basic properties.
    m_playlistListEditor->setContentsMargins(0, 0, 0, 0);
    m_playlistListEditor->setMinimumHeight(knDpi->height(34));

    //Initial the editor buttons.
    for(int i=0; i<EditorButtonCount; i++)
    {
        //Initial the button.
        m_buttons[i]=new KNOpacityAnimeButton(this);
        //Set the fixed size of the button.
        m_buttons[i]->setFixedSize(knDpi->size(15, 15));
    }
    m_buttons[Add]->setIcon(
                QIcon(":/plugin/music/playlist/editor_add.png"));
    m_buttons[RemoveCurrent]->setIcon(
                QIcon(":/plugin/music/playlist/editor_remove.png"));
    m_buttons[Configure]->setIcon(
                QIcon(":/plugin/music/playlist/editor_edit.png"));

    //Generate the add menu and configure menu.
    KNAnimationMenu *addMenu=generateEditorMenu(),
                    *configureMenu=generateEditorMenu();

    //Link the menu with the button.
    connect(m_buttons[Add], &KNOpacityAnimeButton::clicked,
            [=]
            {
                //Set the mouse pressed position.
                addMenu->setMouseDownPos(QCursor::pos());
                //Show the add menu.
                addMenu->exec(QCursor::pos());
            });
    connect(m_buttons[RemoveCurrent], &KNOpacityAnimeButton::clicked,
            this, &KNMusicPlaylistList::requireRemovePlaylist);
    connect(m_buttons[Configure], &KNOpacityAnimeButton::clicked,
            [=]
            {
                //Set the mouse pressed position.
                configureMenu->setMouseDownPos(QCursor::pos());
                //Show the add menu.
                configureMenu->exec(QCursor::pos());
            });

    //Initial the add actions.
    for(int i=0; i<AddMenuActionCount; i++)
    {
        //Generate the actions.
        m_addActions[i]=new QAction(this);
        //Add the action to add menu.
        addMenu->addAction(m_addActions[i]);
    }
    //Insert separator.
    addMenu->insertSeparator(m_addActions[ImportPlaylist]);
    //Link action signals.
    connect(m_addActions[AddPlaylist], &QAction::triggered,
            this, &KNMusicPlaylistList::requireCreatePlaylist);
    connect(m_addActions[ImportPlaylist], &QAction::triggered,
            this, &KNMusicPlaylistList::requireImportPlaylists);
    //Initial the configure actions.
    for(int i=0; i<ConfigureMenuActionCount; i++)
    {
        //Generate the actions.
        m_configureAction[i]=new QAction(this);
        //Add the action to configure menu.
        configureMenu->addAction(m_configureAction[i]);
    }
    //Add actions to menu.
    configureMenu->insertSeparator(m_configureAction[CopyPlaylist]);
    //Link action signals.
    connect(m_configureAction[ExportPlaylist], &QAction::triggered,
            this, &KNMusicPlaylistList::requireExportPlaylist);
    connect(m_configureAction[CopyPlaylist], &QAction::triggered,
            this, &KNMusicPlaylistList::requireCopyPlaylist);

    //Initial the layout of the editor.
    QBoxLayout *editorLayout=new QBoxLayout(QBoxLayout::LeftToRight,
                                            m_playlistListEditor);
    editorLayout->setContentsMargins(knDpi->margins(8, 8, 8, 8));
    editorLayout->setSpacing(knDpi->width(8));
    m_playlistListEditor->setLayout(editorLayout);
    //Add button to layout.
    editorLayout->addWidget(m_buttons[Add], 0, Qt::AlignCenter);
    editorLayout->addWidget(m_buttons[RemoveCurrent], 0, Qt::AlignCenter);
    editorLayout->addStretch();
    editorLayout->addWidget(m_buttons[Configure], 0, Qt::AlignCenter);
}

