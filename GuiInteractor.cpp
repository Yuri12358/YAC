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
		qml->setContextProperty("guiInteractor", this);
		qml->setContextProperty("fileModel", &m_archFileModel);
	}

	EntryInfo* GuiInteractor::formEntry(QUrl url, struct EntryInfo* parent)
	{
		return formEntry(url.toLocalFile(), parent);
	}

	EntryInfo* GuiInteractor::formEntry(QString name, EntryInfo* parent)
	{
		QFileInfo info(name);
		EntryInfo* result = new EntryInfo();
		result->name = toUserFriendlyFileName(name);
		result->parent = parent;
		if (info.isFile())
		{
			result->type = EntryType::File;
			result->sizeUncompressed = info.size();
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
					result->children.push_back(formEntry(name + '/' + child, result));
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
		for (const auto& url : urls)
		{
			m_archFileModel.addEntry(formEntry(url));
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