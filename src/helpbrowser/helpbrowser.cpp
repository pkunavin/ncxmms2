/**
 *  This file is a part of ncxmms2, an XMMS2 Client.
 *
 *  Copyright (C) 2011-2015 Pavel Kunavin <tusk.kun@gmail.com>
 *
 *  This program is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU General Public License
 *  as published by the Free Software Foundation; either version 2
 *  of the License, or (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 */

#include "helpbrowser.h"
#include "../hotkeys.h"

#include "../lib/htmlparser.h"

using namespace ncxmms2;

HelpBrowser::HelpBrowser(const Rectangle& rect, Window *parent) :
    TextView(rect, parent)
{
    loadPalette("HelpBrowser");
    setName("Help");
    
    struct KeyDescription {const char *description; KeyEvent::key_t key;};

    const KeyDescription screensSwitchingKeys[] =
    {
        {"Help",                      Hotkeys::Screens::Help::Activate                  },
        {"Active playlist",           Hotkeys::Screens::ActivePlaylist::Activate        },
        {"Local file system browser", Hotkeys::Screens::LocalFileSystemBrowser::Activate},
        {"Server side browser",       Hotkeys::Screens::ServerSideBrowser::Activate     },
        {"Medialib browser",          Hotkeys::Screens::MedialibBrowser::Activate       },
        {"Playlists browser",         Hotkeys::Screens::PlaylistsBrowser::Activate      },
        {"Equalizer",                 Hotkeys::Screens::Equalizer::Activate             },
        {nullptr, 0}
    };

    const KeyDescription listViewKeys[] =
    {
        {"Move cursor up",                       KeyEvent::KeyUp    },
        {"Move cursor down",                     KeyEvent::KeyDown  },
        {"Move cursor to the first item",        KeyEvent::KeyHome  },
        {"Move cursor to the last item",         KeyEvent::KeyEnd   },
        {"Toggle selection",                     KeyEvent::KeyInsert},
        {"Invert selection",                     '*'                },
        {"Select items by regular expression",   '+'                },
        {"Unselect items by regular expression", '\\'               },
        {"Jump to next selected item",           '.'                },
        {"Jump to previous selected item",       ','                },
        {nullptr, 0}
    };

    const KeyDescription playbackControlKeys[] =
    {
        {"Toggle playback", Hotkeys::Playback::Toggle      },
        {"Stop",            Hotkeys::Playback::Stop        },
        {"Next track",      Hotkeys::Playback::Next        },
        {"Previous track",  Hotkeys::Playback::Prev        },
        {"Seek forward",    Hotkeys::Playback::SeekForward },
        {"Seek backward",   Hotkeys::Playback::SeekBackward},
        {nullptr, 0}
    };

    const KeyDescription playlistViewKeys[] =
    {
        {"Remove entry",                 Hotkeys::PlaylistView::RemoveEntry             },
        {"Clear playlist",               Hotkeys::PlaylistView::ClearPlaylist           },
        {"Shuffle playlist",             Hotkeys::PlaylistView::ShufflePlaylist         },
        {"Go to currently playing song", Hotkeys::PlaylistView::GoToCurrentlyPlayingSong},
        {"Move selected songs",          Hotkeys::PlaylistView::MoveSelectedSongs       },
        {"Add file or directory",        Hotkeys::PlaylistView::AddFileOrDirectory      },
        {"Add url",                      Hotkeys::PlaylistView::AddUrl                  },
        {"Show song info",               Hotkeys::PlaylistView::ShowSongInfo            },
        {nullptr, 0}
    };

    namespace FileSystemBrowser = Hotkeys::Screens::FileSystemBrowser;
    const KeyDescription fileSystemBrowserKeys[] =
    {
        {"Play item in active playlist", KeyEvent::KeyEnter                        },
        {"Add item to active playlist",  FileSystemBrowser::AddItemToActivePlaylist},
        {"Go up",                        FileSystemBrowser::GoUp                   },
        {"Change directory",             FileSystemBrowser::ChangeDirectory        },
        {"Reload directory",             FileSystemBrowser::ReloadDirectory        },
        {"Show song info",               FileSystemBrowser::ShowSongInfo           },
        {nullptr, 0}
    };

    namespace MedialibBrowser = Hotkeys::Screens::MedialibBrowser;
    const KeyDescription medialibBrowserKeys[] =
    {
        {"Play item in active playlist", KeyEvent::KeyEnter                      },
        {"Add item to active playlist",  MedialibBrowser::AddItemToActivePlaylist},
        {"Refresh",                      MedialibBrowser::Refresh                },
        {"Show song info",               MedialibBrowser::ShowSongInfo           },
        {nullptr, 0}
    };

    namespace PlaylistsBrowser = Hotkeys::Screens::PlaylistsBrowser;
    const KeyDescription playlistsBrowserKeys[] =
    {
        {"Create new playlist",             PlaylistsBrowser::CreateNewPlaylist          },
        {"Remove playlist",                 PlaylistsBrowser::RemovePlaylist             },
        {"Rename playlist",                 PlaylistsBrowser::RenamePlaylist             },
        {"Go to currently active playlist", PlaylistsBrowser::GoToCurrentlyActivePlaylist},
        {nullptr, 0}
    };

    const struct Section {const char *name; const KeyDescription *keys;} help[] =
    {
        {"Screens switching",   screensSwitchingKeys },
        {"Playback control",    playbackControlKeys  },
        {"List view",           listViewKeys         },
        {"Playlist view",       playlistViewKeys     },
        {"File system browser", fileSystemBrowserKeys},
        {"Medialib browser",    medialibBrowserKeys  },
        {"Playlists browser",   playlistsBrowserKeys }
    };

    std::string text;
    text.append("<pre>");
    for (const Section& section : help) {
        text.append("<b>      ").append(section.name).append("\n");
        text.append("---------------------------------</b>\n");
        const KeyDescription *keyDesc = section.keys;
        while (keyDesc->description) {
            std::string line(5, ' ');
            line.append(KeyEvent(keyDesc->key).keyName());
            line.resize(15, ' ');
            line.append(" : ");
            line.append(keyDesc->description);
            line.append("\n");
            text.append(HtmlParser::encodeEntities(line));
            ++keyDesc;
        }
        text.append("\n");
    }
    text.append("</pre>");
    
    setMode(Mode::RichText);
    setText(text);
}

HelpBrowser::~HelpBrowser()
{
    
}
