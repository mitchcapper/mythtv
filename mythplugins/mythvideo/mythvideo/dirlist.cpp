#include <qdir.h>
#include <iostream>
using namespace std;

#include <qapplication.h>
#include <qsqldatabase.h>
#include <unistd.h>

#include "metadata.h"
#include "databasebox.h"
#include "dirlist.h"

#include <mythtv/themedmenu.h>
#include <mythtv/mythcontext.h>

Metadata * Dirlist::CheckFile(QString &filename)
{
    QString s = filename.section( '/',-1); // s == "myapp"

    Metadata *retdata = new Metadata(filename, s, "album", "title", "genre",
                                     1900, 3, 40);
    return(retdata);
}

Dirlist::Dirlist(QString &directory)
{
    QDir d(directory);
    d.setSorting(QDir::DirsFirst| QDir::Name | QDir::IgnoreCase);
    Metadata *data;
    if (!d.exists())
    {
        cerr << "mythvideo: start directory is not valid" << endl ; 
        return;
    }
     
    const QFileInfoList *list = d.entryInfoList();
    if (!list)
      ;

    QFileInfoListIterator it(*list);
    QFileInfo *fi;

    while ((fi = it.current()) != 0)
    {
        if (fi->fileName() == ".")
        {
            ++it;
	    continue;
        }

        if (fi->fileName() == "..")
        {
            // Don't add parent directory if we're already at the top of
            // the current tree -- don't want MythVideo users randomly
            // browsing the host file system!
            if (0 == directory.compare(gContext->GetSetting("StartDir")))
            {
                ++it;
                continue;
            }
            else
            {
                Metadata retdata(fi->absFilePath(), fi->absFilePath(), "album",
                                 QObject::tr("(Up one level)"), "dir",
                                 1900, 3, 40);
                playlist.prepend(retdata);
                ++it;
                continue;
            }
        }
	
        QString filename = fi->absFilePath();
        if (fi->isDir())
        {
            QString s = filename.section( '/',-1);
	    Metadata retdata(filename, filename, "album", s, "dir",1900, 3, 40);
            playlist.prepend(retdata);
        }
        else
        {
            QString ext = filename.section('.',-1);
	    //printf("profile:%s\n",gContext->GetSetting("Profile").ascii());
	    QString prof = gContext->GetSetting("Profile");
	    if (prof.contains(ext, FALSE))
            {
                data = CheckFile(filename);
                playlist.prepend(*data);
                delete data;
	    }
	}

        ++it;
    }
}

