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

/**
 * A class, that allows interaction between GUI and archiver backend.
 */
class GuiInteractor : public QObject
{
    Q_OBJECT

    ArchivedFileModel* m_archFileModel;
	QTranslator* m_uaTranslator;
	QQmlEngine* m_qmlEngine;

	/**
	 * An enumeration, that represents language, supported byt program.
	 */
	enum class SupportedLanguage
	{
		Default = 0, // English language
		Ukrainian,   // Ukrainian language
		Invalid      // Not supported language
	};

	/**
	 * A structure, specific for GuiInteractor, that represents a set
	 * of global program properties, such as language or showing
	 * detailed file description.
	 */
	struct Settings
	{
		QString settingsFileName = "config.yacc";
		bool showEnhancedFileInfo = false;
		SupportedLanguage language = SupportedLanguage::Default;

		/**
		 * Loads setting from file.
		 */
		void load();

		/**
		 * Stores settings to file.
		 */
		void store();

	} m_settings;

	Compressor m_compressor;
	Extractor m_extractor;
	std::fstream m_currentArchive;
public:
	/**
	 * Constructs interactor instance.
	 *
	 * @param qml qml context for this object
	 * @param engine qml engine for object to interact with GUI
	 * @param parent object parent
	 */
	GuiInteractor(QQmlContext* qml, QQmlEngine* engine, QObject* parent = Q_NULLPTR);
	~GuiInteractor();

	/**
	 * Signals for each listener, that the user have chosen some archive to open.
	 *
	 * @param url chosen file URL
	 */
    Q_SIGNAL void fireOpenArchive(QUrl url);

	/**
	 * Signals for each listener, that the user wants to add to archive specified
	 * file, files or folder.
	 *
	 * @param urls list of files or one-element list with folder we want to add to the archive
	 */
	Q_SIGNAL void fireAddFilesToArchive(QList<QUrl> urls);

	/**
	 * Signals for all listeners, that the user wants to extract archive contents
	 * to the specified folder.
	 *
	 * @param url url of a folder to which user wants to extract archive contents
	 */
	Q_SIGNAL void fireExtractToFolder(QUrl url);

	/**
	 * Signals for all listeners that we successfully created the archive in the specified folder.
	 *
	 * @param fullPath path to the new archive in the file system
	 */
	Q_SIGNAL void fireNewArchiveCreated(QString fullPath);

	/**
	 * Signals for all listeners, that we successfully formed file tree based on user input and
	 * are ready to start file compression.
	 *
	 * @param files the list of formed file tree nodes, that represent files
	 */
	Q_SIGNAL void fireAddFiles(std::vector<EntryInfo*> files);

	/**
	 * Signals for all listeners that the user wants to create an archive in the specified
	 * folder, having specified name.
	 *
	 * @param parentFolder a folder to create archive in
	 * @param fn chosen archive name
	 */
	Q_SIGNAL void fireNewArchive(QUrl parentFolder, QString fn);

	/**
	 * Signals for all listeners that the user wants to step in some folder in archive
	 * view to check its contents.
	 *
	 * @param name name of the folder user wants to enter
	 */
	Q_SIGNAL void fireEnterFolder(QString name);

	/**
	 * Signals for all listeners that the user wants to step out of current folder
	 * in archive view.
	 */
	Q_SIGNAL void fireGoBack();

	/**
	 * Signals for all listeners that we encountered some invalid situation and
	 * want to show message about this.
	 *
	 * @param error text of the error we encountered
	 */
	Q_SIGNAL void fireShowErrorDialog(QString error);

	/**
	 * Signals for all listeners that we have successfully opened an archive with specified
	 * file tree inside.
	 *
	 * @param root the root of the file tree inside the archive we opened
	 */
	Q_SIGNAL void setFileTree(EntryInfo* root);

	/**
	 * A property, that represents a bool flag, whether to show enhanced file description
	 * in archive view.
	 */
	Q_PROPERTY(bool showEnhFileInfo READ getShowEnhFileInfo WRITE setShowEnhFileInfo NOTIFY fireChangeShowEnhFileInfo);

	/**
	 * A property, that represents a language, set for a whole program.
	 */
	Q_PROPERTY(int language READ getLanguage WRITE setLanguage NOTIFY fireChangeLanguage);

	/**
	 * Gets a bool flag, whether to show enhanced file info or not.
	 *
	 * @return whether to show detailed file info or not
	 */
	bool getShowEnhFileInfo();

	/**
	 * Sets a bool flag, whether to show detailed file description.
	 *
	 * @param show show detailed description or not
	 */
	void setShowEnhFileInfo(bool show);

	/**
	 * Gets integral representation of a SupportedLanguage enum with current selected language.
	 *
	 * @return integral representation of a current language
	 */
	int getLanguage();

	/**
	 * Sets an integral representation of a SupportedLanguage enum as a current language.
	 *
	 * @param lang the language we want out program to be translated in
	 */
	void setLanguage(int lang);

	/**
	 * Signals for all listeners, that our option to show detailed file description in archive
	 * view has changed.
	 *
	 * @param show show detailed description or not
	 */
	Q_SIGNAL void fireChangeShowEnhFileInfo(bool show);

	/**
	 * Signals for all listeners, that our program language has changed.
	 *
	 * @param lang new program language
	 */
	Q_SIGNAL void fireChangeLanguage(int lang);

private:
	/**
	 * Reacts to message from GUI to create a new archive: creates and opens archive, and in
	 * case of some error tells user, what went wrong.
	 *
	 * @param parentFolder parent folder for archive to be created
	 * @param fn name of the archive to be created
	 */
	Q_SLOT void onFireNewArchive(QUrl parentFolder, QString fn);

	/**
	 * React to message from GUI to add files to archive: processes given file list, appends
	 * files, and in case of error tells user, what went wrong.
	 *
	 * @param urls list of files or one-element list of folders to add to archive
	 */
	Q_SLOT void onFireAddFilesToArchive(QList<QUrl> urls);

	/**
	 * Reacts to message from GUI to open an existing archive: reads archive header, signals
	 * to display contents, and in case of error tells user, what went wrong.
	 *
	 * @param url path to the archive to open, represented as URL
	 */
	Q_SLOT void onFireOpenArchive(QUrl url);;

	/**
	 * Reacts to user will to enter some folder in the archive view.
	 *
	 * @param name name of the folder to visually enter
	 */
	Q_SLOT void onFireEnterFolder(QString name);

	/**
	 * Reacts to user will to go to the parent of a current directory in an archive view.
	 */
	Q_SLOT void onFireGoBack();

	/**
	 * Reacts to opening an archive with specified file tree inside: visually displays it.
	 *
	 * @param root root of the specified file tree
	 */
	Q_SLOT void onSetFileTree(EntryInfo* root);

	/**
	 * Reacts to the signal to start file compression.
	 *
	 * @param files list of files to be compressed and added to current archive
	 */
	Q_SLOT void onFireAddFiles(std::vector<EntryInfo*> files);

	/**
	 * Reacts to creation of a new archive: opens this archive.
	 *
	 * @param fullPath path to the archive created
	 */
	Q_SLOT void onFireNewArchiveCreated(QString fullPath);

	/**
	 * Reacts to user request to extract current archive into specified folder.
	 *
	 * @param folder folder to extract archive into
	 */
	Q_SLOT void onExtractToFolder(QUrl folder);

	/**
	 * Converts full or partial file path to user-readable file name.
	 *
	 * @param name full or partial path to the file
	 * @return file name in user-readable format
	 */
	static QString toUserFriendlyFileName(QString name);

	/**
	 * Forms file tree from folder or file specified, and optionally connects it as a child
	 * to the other tree.
	 *
	 * @param url url of a file or folder in file system, from which start forming tree
	 * @param files list of file nodes in a formed tree
	 * @param parent optional parent of this file tree
	 * @return root node of formed file tree
	 */
	EntryInfo* formEntry(QUrl url, std::vector<EntryInfo*>& files, EntryInfo* parent = nullptr);

	/**
	 * Forms file tree from folder or file specified, and optionally connects it as a child
	 * to the other tree.
	 *
	 * @param name name of a file or folder in file system, from which start forming tree
	 * @param files list of file nodes in a formed tree
	 * @param parent optional parent of this file tree
	 * @return root node of formed file tree
	 */
	EntryInfo* formEntry(QString name, std::vector<EntryInfo*>& files, EntryInfo* parent = nullptr);
};
}

#endif // GUIINTERACTOR_HPP
