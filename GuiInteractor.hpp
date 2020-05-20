#ifndef GUIINTERACTOR_HPP
#define GUIINTERACTOR_HPP

#include <QQmlContext>
#include <QQmlEngine>
#include "ArchivedFileModel.hpp"

namespace yac
{
class GuiInteractor : public QObject
{
    Q_OBJECT

    ArchivedFileModel m_archFileModel;
public:
	GuiInteractor(QQmlContext* qml, QObject* parent = Q_NULLPTR);
	// QML to C++
    Q_SIGNAL void fireOpenArchive(QUrl url);
	Q_SIGNAL void fireAddFilesToArchive(QList<QUrl> urls); // add file, files or folder
	Q_SIGNAL void fireRemoveFilesFromArchive(QList<QUrl> url); // remove random mix of files/folders
	Q_SIGNAL void fireExtractToFolder(QUrl url);
	Q_SIGNAL void fireConcatWith(QUrl url);
	Q_SIGNAL void fireCancelCurrentArchivation();
	Q_SIGNAL void fireAddFiles(std::vector<EntryInfo*> files);
	// QML to C++ GUI-only
	Q_SIGNAL void fireEnterFolder(QString name);
	Q_SIGNAL void fireGoBack();
	// C++ to QML
	Q_SIGNAL void setFileTree(EntryInfo* root); // set initial file tree on archive opening
	Q_SIGNAL void addEntryToCurrentFolder(EntryInfo* entry); // call when progress dialog for this entry is just hidden
	Q_SIGNAL void setProgress(float progress);

private:
	Q_SLOT void onFireAddFiles(QList<QUrl> urls);
	Q_SLOT void onFireOpenArchive(QUrl url);;
	Q_SLOT void onFireEnterFolder(QString name);
	Q_SLOT void onFireGoBack();
	Q_SLOT void onSetFileTree(EntryInfo* root);
	Q_SLOT void onAddEntryToCurrentFolder(EntryInfo* entry);
	static QString toUserFriendlyFileName(QString name);
	EntryInfo* formEntry(QUrl url, std::vector<EntryInfo*>& files, EntryInfo* parent = nullptr);
	EntryInfo* formEntry(QString name, std::vector<EntryInfo*>& files, EntryInfo* parent = nullptr);
};
}

#endif // GUIINTERACTOR_HPP
