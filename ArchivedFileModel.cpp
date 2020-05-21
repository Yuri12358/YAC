#include "ArchivedFileModel.hpp"
#include <cassert>

namespace yac
{
ArchivedFileModel::ArchivedFileModel(QObject* parent):
	QAbstractListModel(parent),
	m_currentFolder(createNewRootFolder())
{}

ArchivedFileModel::~ArchivedFileModel()
{
	delete m_currentFolder;
}

QVariant ArchivedFileModel::data(const QModelIndex& index, int role) const
{
    if (index.isValid())
    {
        const auto row = index.row();
		assert(m_currentFolder);
		if ((row >= 0) && (row < m_currentFolder->children.size()))
        {
			EntryInfo* needed = m_currentFolder->children[row];
			assert(needed);
            switch (static_cast<Roles>(role))
            {
                case Roles::EntryType:
					return static_cast<int>(needed->type);
                case Roles::EntryName:
					return needed->name;
                case Roles::SizeUncompressed:
					return QString("%1").arg(needed->sizeUncompressed.value);
                case Roles::SizeCompressed:
					return QString("%1").arg(needed->sizeCompressed.value);
                default:
                return QVariant{};
            }
        }
    }
    return QVariant{};
}

Qt::ItemFlags ArchivedFileModel::flags(const QModelIndex& index) const
{
    return Qt::ItemIsEnabled;
}

int ArchivedFileModel::rowCount(const QModelIndex& index) const
{
	return m_currentFolder != nullptr
		? m_currentFolder->children.size()
		: 0;
}

QHash<int, QByteArray> ArchivedFileModel::roleNames() const
{
    static const QHash<int, QByteArray> roleNames
    {
        { static_cast<int>(Roles::EntryType), "type" },
        { static_cast<int>(Roles::EntryName), "name" },
        { static_cast<int>(Roles::SizeCompressed), "sizeComp" },
        { static_cast<int>(Roles::SizeUncompressed), "sizeUncomp" }
    };
    return roleNames;
}

void ArchivedFileModel::setFileTree(EntryInfo* newFileTree)
{
	beginResetModel();
	if (m_currentFolder != newFileTree) {
		resetToNothing();
		m_currentFolder = newFileTree;
	}
	endResetModel();
}

void ArchivedFileModel::addEntry(EntryInfo* info)
{
	assert(m_currentFolder);
	assert(info);
	info->parent = m_currentFolder;
	beginInsertRows(QModelIndex(), m_currentFolder->children.size(), m_currentFolder->children.size());
	m_currentFolder->children.push_back(info);
	endInsertRows();
}

void ArchivedFileModel::removeEntry(const QString& name)
{
	assert(m_currentFolder);
	int pos = 0;
	for (auto it = m_currentFolder->children.begin(); it != m_currentFolder->children.end(); ++it)
	{
		assert(*it);
        if ((*it)->name == name)
		{
			beginRemoveRows(QModelIndex(), pos, pos);
			delete *it;
			m_currentFolder->children.erase(it);
			endRemoveRows();
            break;
		}
		++pos;
    }
}

void ArchivedFileModel::stepIn(const QString &folderName)
{
	assert(m_currentFolder);
	for (auto it = m_currentFolder->children.begin(); it != m_currentFolder->children.end(); ++it)
	{
		assert(*it);
		if ((*it)->name == folderName)
		{
			beginResetModel();
			m_currentFolder = *it;
			endResetModel();
			break;
		}
	}
}

void ArchivedFileModel::stepOut()
{
	assert(m_currentFolder);
	if (m_currentFolder)
	{
		if (m_currentFolder->parent != nullptr)
		{
			beginResetModel();
			m_currentFolder = m_currentFolder->parent;
			endResetModel();
		}
	}
}

void ArchivedFileModel::clear()
{
	beginResetModel();
	resetToNothing();
	endResetModel();
}

void ArchivedFileModel::resetToNothing()
{
	if (m_currentFolder != nullptr)
	{
		while (m_currentFolder->parent != nullptr) m_currentFolder = m_currentFolder->parent;
		m_currentFolder->children.clear();
		delete m_currentFolder;
		m_currentFolder = nullptr;
	}
}

EntryInfo* ArchivedFileModel::getCurrentEI()
{
	return m_currentFolder;
}

EntryInfo* ArchivedFileModel::createNewRootFolder()
{
	return EntryInfo::newFolder(nullptr, tr("Root folder"));
}

EntryInfo * EntryInfo::newFile(EntryInfo * parent, QString name, UncompressedSize originalSize, CompressedSize compressedSize, PositionInArchive positionInArchive)
{
	const auto file = new EntryInfo;
	file->parent = parent;
	file->type = EntryType::File;
	file->name = std::move(name);
	file->sizeCompressed = compressedSize;
	file->sizeUncompressed = originalSize;
	file->positionInArchive = positionInArchive;
	return file;
};

EntryInfo * EntryInfo::newFolder(EntryInfo * parent, QString name)
{
	const auto folder = new EntryInfo;
	folder->parent = parent;
	folder->type = EntryType::Folder;
	folder->name = std::move(name);
	return folder;
};
}
