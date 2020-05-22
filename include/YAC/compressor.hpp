#ifndef YAC_COMPRESSOR_HPP
#define YAC_COMPRESSOR_HPP

#include<YAC/size_types.hpp>
#include<YAC/byte.hpp>
#include<fstream>
#include<vector>

namespace yac {
	/**
	 * The information about a file or directory to be compressed.
	 */
	struct EntryInfo;

	/**
	 * Performs the compression of files and folders
	 * using the Huffman algorithm.
	 *
	 * @see Extractor
	 */
	class Compressor {
		/**
		 * Type alias for byte frequency counters.
		 */
		using FreqType = unsigned long long;

		/**
		 * Type alias for a Huffman code for a byte.
		 */
		using BitCode = std::vector<bool>;

		/**
		 * The Huffman binary tree helper struct.
		 *
		 * @see m_buildTree()
		 * @see m_tree
		 */
		struct TreeNode {
			/**
			 * The comparator for std::multiset to build the tree.
			 * Compares nodes by their m_freq fields.
			 *
			 * @see m_buildTree()
			 */
			struct Comparator {
				bool operator() (const TreeNode * left, const TreeNode * right) const noexcept;
			};

			/**
			 * True for leaf nodes and false for branch nodes.
			 */
			bool m_isLeaf = false;

			/**
			 * The corresponding byte. Filled only for leaf nodes.
			 * The value is unique for each leaf node.
			 */
			Byte m_value{};

			/**
			 * The total frequency of all the bytes in the subtree.
			 */
			FreqType m_freq = 0;

			/**
			 * The left subtree. Null for the leaf nodes.
			 */
			TreeNode * m_left = nullptr;

			/**
			 * The right subtree. Null for the leaf nodes.
			 */
			TreeNode * m_right = nullptr;

			/**
			 * The constructor for a leaf node.
			 */
			TreeNode(Byte value, FreqType freq);

			/**
			 * The constructor for a branch node.
			 * Sets the m_freq field to the sum of m_freq of the children.
			 */
			TreeNode(TreeNode * a, TreeNode * b);

			/**
			 * Deallocates children
			 */
			~TreeNode();
		};

		/**
		 * The original size of the file being compressed.
		 * Filled with m_calculateFrequencyAndFileSize().
		 *
		 * @see m_calculateFrequencyAndFileSize()
		 */
		unsigned long long m_fileSize = 0;

		/**
		 * The byte frequency table for the Huffman encoding.
		 *
		 * @see m_calculateFrequencyAndFileSize()
		 */
		FreqType m_frequency[256]{};

		/**
		 * The Huffman coding binary tree.
		 *
		 * @see m_buildTree()
		 */
		TreeNode * m_tree = nullptr;

		/**
		 * The Huffman bit codes for bytes. Shorter codes are for more frequent bytes.
		 *
		 * @see m_generateCodes()
		 */
		BitCode m_codes[256];

		/**
		 * Calculates the frequency of each byte in the stream by reading the stream until the end.
		 * Fills the m_frequency field.
		 * Also fills the m_fileSize with the amount of read bytes.
		 * As a post-condition, the file cursor is reset to the beginning and the file error flags are cleared.
		 *
		 * @param in The input file. Should be opened in binary mode.
		 *           After the call the cursor is set to the beginning, and the error flags are cleared.
		 *
		 * @see m_fileSize
		 * @see m_frequency
		 */
		void m_calculateFrequencyAndFileSize(std::istream & in);

		/**
		 * Builds the Huffman coding tree using the byte frequency data from m_frequency field.
		 * Fills the m_tree field.
		 *
		 * @see m_tree
		 * @see m_frequency
		 * @see m_calculateFrequencyAndFileSize()
		 */
		void m_buildTree();

		/**
		 * Computes the Huffman codes by a recursive traversal of the Huffman tree stored in m_tree.
		 * Fills the m_codes field.
		 *
		 * @see m_codes
		 * @see m_tree
		 * @see m_visitNode
		 */
		void m_generateCodes();

		/**
		 * Computes the Huffman codes by a recursive traversal of the Huffman tree.
		 * Fills the m_codes field.
		 * Should not be called directly, just a helper for m_generateCodes.
		 *
		 * @param node The currently traversed node.
		 * @param buffer A helper buffer.
		 *               For the non-recursive call of this method should be empty.
		 *               For the leaf nodes it is stored to the corresponding element of the m_codes as-is.
		 *               Otherwise it is forwarded to the childen,
		 *               with 0 and 1 bits appended for the left and right children respectively.
		 *               After the call the buffer content remains the same as before the call.
		 *
		 * @see m_generateCodes
		 * @see m_codes
		 */
		void m_visitNode(const TreeNode * node, BitCode & buffer);

		/**
		 * Writes the file metadata header to the archive EXCEPT the compressed size
		 * (it should be written with m_writeCompressedSize).
		 * Computes and writes the Huffman tree for the file.
		 * Writes:
		 *  - 64bit uncompressed file size;
		 *  - 64bit of padding for the compressed size;
		 *  - 32bit length of the file path;
		 *  - the internal file path in the archive as 8bit char sequence.
		 *  - the computed Huffman tree
		 *
		 * @param fileInfo The info about the file that is being archived.
		 *                 Its uncompressed size and name should be filled.
		 * @param out The output stream representing the archive.
		 *            Should be opened in the binary mode, the behavior is undefined otherwise.
		 * @return The amount of bytes written to the archive.
		 */
		HuffmanTreeSize m_writeHeader(const EntryInfo & fileInfo, std::ostream & out);

		/**
		 * Serializes the Huffman tree node to the archive recursively.
		 *
		 * @param node The tree node to serialize. Should not be null.
		 * @param out The output stream representing the archive.
		 *            Should be opened in the binary mode, the behavior is undefined otherwise.
		 * @return The amount of bytes written to the archive.
		 */
		HuffmanTreeSize m_printNode(const TreeNode * node, std::ostream & out);

		/**
		 * Fills the compressed size part of the file header.
		 * Leaves the out file cursor right after the written data.
		 *
		 * @param out The output stream representing the archive.
		 *            Should be opened in the binary mode, the behavior is undefined otherwise.
		 * @param headerStartPos The position, where the file header is located in the archive.
		 * @param compressedSize The data to write.
		 */
		void m_writeCompressedSize(std::ostream & out, std::ostream::pos_type headerStartPos, CompressedSize compressedSize);

		/**
		 * Compresses the file contents and writes the compressed data to the archive.
		 * Doesn't write any metadata.
		 *
		 * @param in The input stream representing the file that is being added. Should be opened in binary mode.
		 * @param out The output stream representing the archive.
		 *            Should be opened in the binary mode, the behavior is undefined otherwise.
		 * @return The amount of bytes written to the archive.
		 */
		CompressedContentSize m_encode(std::istream & in, std::ostream & out);

		/**
		 * Adds a file (not a directory) to the archive stream.
		 * Fills out the compressed size and position in archive of the added file.
		 *
		 * @param fileInfo Information about the file to add.
		 *                 The full path, name and uncompressed size should be filled.
		 * @param in The input stream representing the file that is being added. Should be opened in binary mode.
		 * @param out The output stream representing the archive.
		 *            Should be opened in the binary mode, the behavior is undefined otherwise.
		 */
		void m_addFileToArchive(EntryInfo & fileInfo, std::istream & in, std::ostream &out);

	public:
		/**
		 * Adds a file or a directory to the archive stream recursively
		 * and fills out the compressed size and position in archive of the added files.
		 *
		 * @param entry Information about the file or directory to add.
		 *              The full path, name and uncompressed size should be filled.
		 * @param archive The archive, represented as a stream.
		 *                Should be opened in the binary mode, the behavior is undefined otherwise.
		 */
		void addToArchive(EntryInfo & entry, std::ostream & archive);
	};
}

#endif // YAC_COMPRESSOR_HPP

