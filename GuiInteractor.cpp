#include "GuiInteractor.hpp"
#include <QFileInfo>
#include <QDir>

namespace yac
{
	GuiInteractor::GuiInteractor(QQmlContext* qml, QObject* parent):
		QObject(parent),
		m_archFileModel(this)
	{
		connect(this, &GuiInteractor::fireAddFilesToArchive, this, &GuiInteractor::onFireAddFiles);
		connect(this, &GuiInteractor::fireEnterFolder, this, &GuiInteractor::onFireEnterFolder);
		connect(this, &GuiInteractor::fireGoBack, this, &GuiInteractor::onFireGoBack);
		connect(this, &GuiInteractor::fireNewArchive, this, &GuiInteractor::onFireNewArchive);
		qml->setContextProperty("guiInteractor", this);
		qml->setContextProperty("fileModel", &m_archFileModel);
	}

	EntryInfo* GuiInteractor::formEntry(QUrl url, std::vector<EntryInfo*>& files, EntryInfo* parent)
	{
		return formEntry(url.toLocalFile(), files, parent);
	}

	EntryInfo* GuiInteractor::formEntry(QString name, std::vector<EntryInfo*>& files, EntryInfo* parent)
	{
		QFileInfo info(name);
		EntryInfo* result = new EntryInfo();
		result->name = toUserFriendlyFileName(name);
		result->fullPath = name;
		result->parent = parent;
		if (info.isFile())
		{
			result->type = EntryType::File;
			result->sizeUncompressed = info.size();
			files.push_back(result);
		}
		else if (info.isDir())
		{
			result->type = EntryType::Folder;
			QDir dir(name);
			auto children = dir.entryList();
			for (auto child : children)
			{
				if (child != QString(".") && child != QString(".."))
				{
					result->children.push_back(formEntry(name + '/' + child, files, result));
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
		m_archFileModel.clear();
		m_archFileModel.setFileTree(root);
	}

	void GuiInteractor::onAddEntryToCurrentFolder(EntryInfo* entry)
	{
		m_archFileModel.addEntry(entry);
	}

	void GuiInteractor::onFireOpenArchive(QUrl url)
	{
		EntryInfo* entry = nullptr/*to be implemented*/;
		onSetFileTree(entry);
	}

	void GuiInteractor::onFireAddFiles(QList<QUrl> urls)
	{
		std::vector<EntryInfo*> files;
		for (const auto& url : urls)
		{
			formEntry(url, files);
		}
		Q_EMIT fireAddFiles(files);
	}

	void GuiInteractor::onFireNewArchive(QUrl url, QString fn)
	{
		if (fn == "") return;
		auto path = url.toLocalFile() + '/' + fn + ".yac";
		QFile file(path);
		if (file.open(QIODevice::ReadWrite))
		{
			file.close();
			Q_EMIT fireNewArchiveCreated(path);
		}
	}

	void GuiInteractor::onFireEnterFolder(QString name)
	{
		m_archFileModel.stepIn(name);
	}

	void GuiInteractor::onFireGoBack()
	{
		m_archFileModel.stepOut();
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

}
