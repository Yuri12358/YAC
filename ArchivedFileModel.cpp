#include "ArchivedFileModel.hpp"
#include <cassert>

namespace yac
{
ArchivedFileModel::ArchivedFileModel(QObject* parent):
	QAbstractListModel(parent)
{
	m_currentFolder = new EntryInfo();
	m_currentFolder->parent = nullptr;
	m_currentFolder->children = {};
	m_currentFolder->type = EntryType::Folder;
	m_currentFolder->name = tr("Root folder");
}

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
			EntryInfo* needed = nullptr;
			needed = m_currentFolder->children[row];
			assert(needed);
            switch (static_cast<Roles>(role))
            {
                case Roles::EntryType:
					return (int)needed->type;
                case Roles::EntryName:
					return needed->name;
                case Roles::SizeUncompressed:
					return QString("%1").arg(needed->sizeUncompressed);
                case Roles::SizeCompressed:
					return QString("%1").arg(needed->sizeCompressed);
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
	assert(m_currentFolder);
	return m_currentFolder->children.size();
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
	assert(m_currentFolder);
	beginResetModel();
	resetToNothing();
	m_currentFolder = newFileTree;
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
	assert(m_currentFolder);
	if (m_currentFolder != nullptr)
	{
		while (m_currentFolder->parent != nullptr) m_currentFolder = m_currentFolder->parent;
		m_currentFolder->children.clear();
	}
}

}
