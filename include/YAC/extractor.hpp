#ifndef YAC_EXTRACTOR_HPP
#define YAC_EXTRACTOR_HPP

#include<YAC/byte.hpp>
#include<ostream>
#include<istream>
#include "ArchivedFileModel.hpp"

namespace yac {
	/**
	 * Performs reading and decompression of archives.
	 * Uses the Huffman algorithm.
	 *
	 * @see Compressor
	 */
	class Extractor {
		/**
		 * The Huffman binary tree helper struct.
		 *
		 * @see m_buildTree()
		 * @see m_tree
		 */
		struct TreeNode {
			/**
			 * The left subtree. Null for the leaf nodes.
			 */
			TreeNode * m_left{ nullptr };

			/**
			 * The right subtree. Null for the leaf nodes.
			 */
			TreeNode * m_right{ nullptr };

			/**
			 * The size of deserialized node in bytes.
			 */
			HuffmanTreeSize m_size{};

			/**
			 * True for leaf nodes and false for branch nodes.
			 */
			bool m_isLeaf{ false };

			/**
			 * The corresponding byte. Filled only for leaf nodes.
			 * The value is unique for each leaf node.
			 */
			Byte m_value{};

			/**
			 * Dumps the subtree to the stream. For debug.
			 */
			void dump(std::ostream & to);

			/**
			 * The constructor for a leaf node.
			 */
			TreeNode(Byte value);

			/**
			 * The constructor for a branch node.
			 */
			TreeNode(TreeNode * a, TreeNode * b);

			/**
			 * Deallocates children nodes recursively.
			 */
			~TreeNode();
		};

		/**
		 * Helper struct for a file metadata read from archive.
		 *
		 * @see m_readFileHeader()
		 */
		struct FileHeader {
			/**
			 * The size of a file before compression
			 */
			UncompressedSize originalSize{ 0 };

			/**
			 * The size of a file after compression with a Huffman tree.
			 */
			CompressedSize compressedSize{ 0 };

			/**
			 * The internal path of a file relative to the archive root.
			 */
			std::string path;
		};

		/**
		 * The Huffman coding tree.
		 *
		 * @see m_readNode()
		 */
		TreeNode * m_tree = nullptr;

		/**
		 * Throws an exception with description of the error.
		 *
		 * @param error The error description body.
		 * @throws std::runtime_error
		 */
		void m_fail(std::string_view error);

		/**
		 * Reads a file metadata from an archive.
		 *
		 * @param in The archive stream to read from.
		 *           Should be opened in binary mode.
		 * @return The read file info.
		 */
		FileHeader m_readFileHeader(std::istream & in);

		/**
		 * Recursively deserializes the Huffman coding tree from an archive.
		 *
		 * @param in The archive stream to read from.
		 *           Should be opened in binary mode.
		 * @return The deserialized tree allocated with new operator.
		 *         Should be deallocated with delete.
		 * @throws std::runtime_error In case of reading failure.
		 */
		TreeNode * m_readNode(std::istream & in);

		/**
		 * Decompressed the compressed contents from an archive.
		 * Tries to output exactly finalSize bytes.
		 *
		 * @param in The archive stream to read from.
		 *           Should be opened in binary mode.
		 * @param out The destination stream.
		 *            Should be opened in binary mode.
		 * @param finalSize The expected length of decompressed data.
		 * @throws std::runtime_error In case of reading failure.
		 */
		void m_decode(std::istream & in, std::ostream & out, UncompressedSize finalSize);

		/**
		 * Adds a new file record to the internal archive structure.
		 *
		 * @param metadataRoot The folder, relative to which the file should be put.
		 * @param fileInfo The file metadata to record in the internal file structure.
		 * @param pos The position of the file metadata header in the archive
		 * @see extractMetaInfo()
		 */
		void m_addMetadata(EntryInfo & metadataRoot, const FileHeader & fileInfo, PositionInArchive pos);

		/**
		 * Extracts a file (not a directory) from the archive to a stream.
		 *
		 * @param what The file to extract
		 * @param from The archive stream to extract from.
		 *             Should be opened in binary mode.
		 * @param to The destination stream.
		 *           Should be opened in binary mode.
		 * @throws std::runtime_error in case of a reading error
		 */
		void m_extract(const EntryInfo & what, std::istream & from, std::ostream & to);

	public:
		/**
		 * Extracts the files from the archive to a given path.
		 * Creates the needed intermediate folders if they don't exist.
		 *
		 * @param what The file or the directory to extract
		 * @param from The archive stream to extract from.
		 *             Should be opened in binary mode.
		 * @param where The extraction root path.
		 * @throws std::runtime_error in case of a reading error
		 */
		void extract(const EntryInfo & what, std::istream & from, const std::string & where);

		/**
		 * Builds the internal file structure of a given archive.
		 *
		 * @param archive The archive stream to extract from.
		 *                Should be opened in binary mode.
		 * @return The info about the archive internal structure
		 *         allocated with the new operator.
		 *         Should be manually deallocated with delete.
		 * @see m_addMetadata()
		 */
		EntryInfo * extractMetaInfo(std::istream & archive);
	};
}

#endif // YAC_EXTRACTOR_HPP

