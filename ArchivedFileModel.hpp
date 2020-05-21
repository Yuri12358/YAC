#ifndef ARCHIVEDFILEMODEL_HPP
#define ARCHIVEDFILEMODEL_HPP

#include <YAC/size_types.hpp>
#include <QAbstractListModel>
#include <vector>

namespace yac
{
    enum class EntryType
    {
        File = 0,
        Folder
    };

	struct EntryInfo
	{
		EntryInfo() = default;
		~EntryInfo()
		{
			for (auto ch : children) delete ch;
		};
		EntryInfo(const EntryInfo&) = delete;
		EntryInfo(EntryInfo&) = delete;
		EntryInfo(EntryInfo&&) = delete;
		EntryInfo* parent = nullptr;
		std::vector<EntryInfo*> children;
        EntryType type;
        QString name;
		QString fullPath;
		UncompressedSize sizeUncompressed{ 0 };
		CompressedSize sizeCompressed{ 0 };
		PositionInArchive positionInArchive{ 0 };

		static EntryInfo * newFile(EntryInfo * parent, QString name, UncompressedSize originalSize, CompressedSize compressedSize, PositionInArchive positionInArchive);
		static EntryInfo * newFolder(EntryInfo * parent, QString name);

		EntryInfo& operator=(EntryInfo) = delete;
		EntryInfo& operator=(const EntryInfo&) = delete;
		EntryInfo& operator=(EntryInfo&) = delete;
		EntryInfo& operator=(EntryInfo&&) = delete;
    };

    class ArchivedFileModel : public QAbstractListModel
    {
        Q_OBJECT
    public:
        ArchivedFileModel(QObject* parent = Q_NULLPTR);
        virtual ~ArchivedFileModel();

        virtual QVariant data(const QModelIndex& index, int role) const Q_DECL_OVERRIDE;
        virtual Qt::ItemFlags flags(const QModelIndex& index) const Q_DECL_OVERRIDE;
        virtual int rowCount(const QModelIndex& index) const Q_DECL_OVERRIDE;
		virtual QHash<int, QByteArray> roleNames() const Q_DECL_OVERRIDE;
		Q_INVOKABLE void setFileTree(EntryInfo* newFileTree);
		Q_INVOKABLE void addEntry(EntryInfo* info);
		Q_INVOKABLE void removeEntry(const QString& name);
		Q_INVOKABLE void stepIn(const QString& folderName);
		Q_INVOKABLE void stepOut();
        Q_INVOKABLE void clear();

		EntryInfo* getCurrentEI();
        static EntryInfo* createNewRootFolder();

    private:
        enum class Roles
        {
            EntryType = Qt::UserRole + 1,
            EntryName,
            SizeUncompressed,
            SizeCompressed
		};

		void resetToNothing();

		EntryInfo* m_currentFolder; // todo: smart pointers
    };
}

#endif // ARCHIVEDFILEMODEL_HPP
