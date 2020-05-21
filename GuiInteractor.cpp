#include "GuiInteractor.hpp"
// ReSharper disable once CppUnusedIncludeDirective
#include <QFileInfo>
#include <QCoreApplication>
#include <QDir>
#include <iostream>

namespace
{
	void openFile(std::fstream & file, const std::string & path)
	{
		file.close();

		const auto flags = std::ios::in | std::ios::out | std::ios::binary;
		file.open(path, flags);
		if (!file.is_open())
		{
			file.open(path, flags | std::ios::trunc);
		}
	}
}

namespace yac
{
	void GuiInteractor::Settings::load()
	{
		QFile settFile(QCoreApplication::applicationDirPath() + '/' + settingsFileName);
		if (!settFile.exists())
		{
			return;
		}
		settFile.open(QIODevice::ReadOnly);
		auto content = settFile.readAll().toStdString();
		showEnhancedFileInfo = content[0] - '0';
		content = std::string(content.begin() + 2, content.end());
		int lang = std::stoi(content);
		if (lang >= (int)SupportedLanguage::Invalid || lang < 0)
		{
			assert(false);
			settFile.close();
			return;
		}
		language = static_cast<SupportedLanguage>(lang);
		settFile.close();
	}

	void GuiInteractor::Settings::store()
	{
		QFile settFile(QCoreApplication::applicationDirPath() + '/' + settingsFileName);
		settFile.open(QIODevice::ReadWrite);
		if (!settFile.exists())
		{
			assert(false);
			return;
		}
		settFile.write(std::string(std::to_string((int)showEnhancedFileInfo) + ' ' + std::to_string((int)language)).c_str());
		settFile.close();
	}

	GuiInteractor::GuiInteractor(QQmlContext* qml, QQmlEngine* engine, QObject* parent):
		QObject(parent),
		m_archFileModel(new ArchivedFileModel(this)),
		m_uaTranslator(new QTranslator(this)),
		m_qmlEngine(engine)
	{
		connect(this, &GuiInteractor::fireAddFilesToArchive, this, &GuiInteractor::onFireAddFilesToArchive);
		connect(this, &GuiInteractor::fireEnterFolder, this, &GuiInteractor::onFireEnterFolder);
		connect(this, &GuiInteractor::fireGoBack, this, &GuiInteractor::onFireGoBack);
		connect(this, &GuiInteractor::fireNewArchive, this, &GuiInteractor::onFireNewArchive);
		connect(this, &GuiInteractor::fireAddFiles, this, &GuiInteractor::onFireAddFiles);
		connect(this, &GuiInteractor::fireNewArchiveCreated, this, &GuiInteractor::onFireNewArchiveCreated);
		connect(this, &GuiInteractor::fireOpenArchive, this, &GuiInteractor::onFireOpenArchive);
		connect(this, &GuiInteractor::setFileTree, this, &GuiInteractor::onSetFileTree);
		connect(this, &GuiInteractor::fireExtractToFolder, this, &GuiInteractor::onExtractToFolder);

		qml->setContextProperty("guiInteractor", this);
		qml->setContextProperty("fileModel", m_archFileModel);
		m_settings.load();
		setLanguage((int)m_settings.language);
	}

	GuiInteractor::~GuiInteractor()
	{
		m_settings.store();
	}

	EntryInfo* GuiInteractor::formEntry(QUrl url, std::vector<EntryInfo*>& files, EntryInfo* parent)
	{
		if (url.toLocalFile() == "") return nullptr;
		return formEntry(url.toLocalFile(), files, parent);
	}

	EntryInfo* GuiInteractor::formEntry(QString name, std::vector<EntryInfo*>& files, EntryInfo* parent)
	{
		if (name == "") return nullptr;
		const QFileInfo info(name);
		const auto result = new EntryInfo();
		result->name = toUserFriendlyFileName(name);
		result->fullPath = name;
		result->parent = parent;
		if (parent != nullptr) {
			parent->children.push_back(result);
		}
		if (info.isFile())
		{
			result->type = EntryType::File;
			result->sizeUncompressed = UncompressedSize{ static_cast<Size>(info.size()) };
			files.push_back(result);
		}
		else if (info.isDir())
		{
			result->type = EntryType::Folder;
			const QDir dir(name);
			auto children = dir.entryList();
			for (auto & child : children)
			{
				if (child != QString(".") && child != QString(".."))
				{
					formEntry(name + '/' + child, files, result);
				}
			}
		}
		else
		{
			result->name = "invalid";
		}
		return result;
	}

	void GuiInteractor::onSetFileTree(EntryInfo* root)
	{
		if (m_archFileModel->getCurrentEI() != root) {
			m_archFileModel->clear();
		}
		m_archFileModel->setFileTree(root);
	}

	void GuiInteractor::onAddEntryToCurrentFolder(EntryInfo* entry)
	{
		m_archFileModel->addEntry(entry);
	}

	void GuiInteractor::onFireAddFiles(std::vector<EntryInfo*> files)
	{
		for (auto entry : files) {
			m_compressor.compress(*entry, m_currentArchive);
		}
		setFileTree(m_archFileModel->getCurrentEI());
	}

	void GuiInteractor::onFireNewArchiveCreated(QString fullPath)
	{
		const QDir dir(fullPath + "/..");
		(void) dir.mkpath("."); // todo: error handling
		openFile(m_currentArchive, fullPath.toStdString());
		std::cout << "creating a new archive at '" << fullPath.toStdString() << "'\n";
		assert(m_currentArchive.is_open());

		Q_EMIT setFileTree(m_archFileModel->createNewRootFolder());
	}

	void GuiInteractor::onExtractToFolder(QUrl folder)
	{
		assert(m_currentArchive.is_open());
		if (m_currentArchive.is_open() && m_archFileModel->getCurrentEI())
		{
			m_extractor.extract(*m_archFileModel->getCurrentEI(), m_currentArchive, folder.toLocalFile().toStdString());
		}
	}

	void GuiInteractor::onFireOpenArchive(QUrl url)
	{
		const auto path = url.toLocalFile().toStdString();
		openFile(m_currentArchive, path);
		std::cout << "opening an archive at '" << path << "'\n";
		assert(m_currentArchive.is_open());
		Q_EMIT setFileTree(m_extractor.extractMetaInfo(m_currentArchive));
	}

	void GuiInteractor::onFireAddFilesToArchive(QList<QUrl> urls)
	{
		std::vector<EntryInfo*> files;
		for (const auto& url : urls)
		{
			QString fn = toUserFriendlyFileName(url.toLocalFile());
			EntryInfo* currentFolder = m_archFileModel->getCurrentEI();
			bool alreadyExists = false;
			for (EntryInfo* child : currentFolder->children)
			{
				if (fn == child->name)
				{
					Q_EMIT fireShowErrorDialog(tr("This file already exists: ") + fn);
					alreadyExists = true;
					break;
				}
			}
			if (!alreadyExists)
			{
				formEntry(url, files, currentFolder);
			}
		}
		if (!files.empty())
		{
			Q_EMIT fireAddFiles(files);
		}
	}

	void GuiInteractor::onFireNewArchive(QUrl url, QString fn)
	{
		if (fn == "") return;
		const auto path = url.toLocalFile() + '/' + fn + ".yac";
		QFile file(path);
		if (file.open(QIODevice::ReadWrite))
		{
			file.close();
			Q_EMIT fireNewArchiveCreated(path);
		}
	}

	void GuiInteractor::onFireEnterFolder(QString name)
	{
		m_archFileModel->stepIn(name);
	}

	void GuiInteractor::onFireGoBack()
	{
		m_archFileModel->stepOut();
	}

	QString GuiInteractor::toUserFriendlyFileName(QString name)
	{
		int idx = name.lastIndexOf('/');
		++idx;
		QString res;
		for (; idx < name.size(); ++idx)
		{
			res.append(name[idx]);
		}
		return res;
	}

	bool GuiInteractor::getShowEnhFileInfo()
	{
		return m_settings.showEnhancedFileInfo;
	}

	void GuiInteractor::setShowEnhFileInfo(bool show)
	{
		m_settings.showEnhancedFileInfo = show;
	}

	int GuiInteractor::getLanguage()
	{
		return static_cast<int>(m_settings.language);
	}

	void GuiInteractor::setLanguage(int lang)
	{
		auto language = static_cast<SupportedLanguage>(lang);
		m_settings.language = language;
		if (language == SupportedLanguage::Ukrainian)
		{
			bool res = m_uaTranslator->load("yac_uk_UA", QCoreApplication::applicationDirPath());
			assert(res);
			res = qApp->installTranslator(m_uaTranslator);
			assert(res);
		}
		else
		{
			qApp->removeTranslator(m_uaTranslator);
		}

		m_qmlEngine->retranslate();
	}

}
