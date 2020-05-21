#ifndef GUIINTERACTOR_HPP
#define GUIINTERACTOR_HPP

#include <YAC/compressor.hpp>
#include <YAC/extractor.hpp>
#include <QQmlContext>
#include <QQmlEngine>
#include <QFile>
#include "ArchivedFileModel.hpp"
#include <QTranslator>

namespace yac
{
class GuiInteractor : public QObject
{
    Q_OBJECT

    ArchivedFileModel* m_archFileModel;
	QTranslator* m_uaTranslator;
	QQmlEngine* m_qmlEngine;

	enum class SupportedLanguage
	{
		Default = 0, // English
		Ukrainian,
		Invalid
	};

	struct Settings
	{
		QString settingsFileName = "config.yacc";
		bool showEnhancedFileInfo = false;
		SupportedLanguage language = SupportedLanguage::Default;
		void load();
		void store();
	} m_settings;

	Compressor m_compressor;
	Extractor m_extractor;
	std::fstream m_currentArchive;
public:
	GuiInteractor(QQmlContext* qml, QQmlEngine* engine, QObject* parent = Q_NULLPTR);
	~GuiInteractor();
	// QML to C++
    Q_SIGNAL void fireOpenArchive(QUrl url);
	Q_SIGNAL void fireAddFilesToArchive(QList<QUrl> urls); // add file, files or folder
	Q_SIGNAL void fireRemoveFilesFromArchive(QList<QUrl> url); // remove random mix of files/folders
	Q_SIGNAL void fireExtractToFolder(QUrl url);
	Q_SIGNAL void fireConcatWith(QUrl url);
	Q_SIGNAL void fireCancelCurrentArchivation();
	Q_SIGNAL void fireNewArchiveCreated(QString fullPath);
	Q_SIGNAL void fireAddFiles(std::vector<EntryInfo*> files);
	// QML to C++ GUI-only
	Q_SIGNAL void fireNewArchive(QUrl parentFolder, QString fn);
	Q_SIGNAL void fireShowEnhancedFileInfo(bool show);
	Q_SIGNAL void fireEnterFolder(QString name);
	Q_SIGNAL void fireGoBack();
	// C++ to QML but GUI-only
	Q_SIGNAL void fireShowErrorDialog(QString error);
	// C++ to QML
	Q_SIGNAL void setFileTree(EntryInfo* root); // set initial file tree on archive opening
	Q_SIGNAL void addEntryToCurrentFolder(EntryInfo* entry); // call when progress dialog for this entry is just hidden
	Q_SIGNAL void setProgress(float progress);

	// translation stuff and other settings
//	Q_INVOKABLE void selectLang(QString lang);
	Q_PROPERTY(bool showEnhFileInfo READ getShowEnhFileInfo WRITE setShowEnhFileInfo NOTIFY fireChangeShowEnhFileInfo);
	Q_PROPERTY(int language READ getLanguage WRITE setLanguage NOTIFY fireChangeLanguage);
	bool getShowEnhFileInfo();
	void setShowEnhFileInfo(bool show);
	int getLanguage();
	void setLanguage(int lang);
	Q_SIGNAL void fireChangeShowEnhFileInfo(bool show);
	Q_SIGNAL void fireChangeLanguage(int lang);

private:
	Q_SLOT void onFireNewArchive(QUrl parentFolder, QString fn);
	Q_SLOT void onFireAddFilesToArchive(QList<QUrl> urls);
	Q_SLOT void onFireOpenArchive(QUrl url);;
	Q_SLOT void onFireEnterFolder(QString name);
	Q_SLOT void onFireGoBack();
	Q_SLOT void onSetFileTree(EntryInfo* root);
	Q_SLOT void onAddEntryToCurrentFolder(EntryInfo* entry);
	Q_SLOT void onFireAddFiles(std::vector<EntryInfo*> files);
	Q_SLOT void onFireNewArchiveCreated(QString fullPath);
	Q_SLOT void onExtractToFolder(QUrl folder);
	static QString toUserFriendlyFileName(QString name);
	EntryInfo* formEntry(QUrl url, std::vector<EntryInfo*>& files, EntryInfo* parent = nullptr);
	EntryInfo* formEntry(QString name, std::vector<EntryInfo*>& files, EntryInfo* parent = nullptr);
};
}

#endif // GUIINTERACTOR_HPP
