#ifndef INICPP_CONFIG_H
#define INICPP_CONFIG_H

#include <vector>
#include <iostream>
#include <map>

#include "exception.h"
#include "section.h"
#include "option.h"
#include "schema.h"


namespace inicpp
{
	/** Forward declaration, stated because of ring dependencies */
	class schema;
	/** Forward declaration of iterator used in config class */
	template<typename Element> class config_iterator;


	/**
	 * Represents the base object of ini configuration.
	 * Contains list of sections in logical map structure.
	 * Can be constructed directly from string or stream.
	 */
	class config
	{
	private:
		typedef std::vector<std::shared_ptr<section>> sections_vector;
		typedef std::map<std::string, std::shared_ptr<section>> sections_map;
		typedef std::pair<std::string, std::shared_ptr<section>> sections_map_pair;

		/** List of sections in this config instance */
		sections_vector sections_;
		/** Map of sections for better searching */
		sections_map sections_map_;
		/** Corresponding schema if any */
		std::shared_ptr<schema> schema_;

		friend class config_iterator<section>;
		friend class config_iterator<const section>;

	public:
		/** type of iterator */
		typedef config_iterator<section> iterator;
		/** type of const iterator */
		typedef config_iterator<const section> const_iterator;

		/**
		 * Default constructor.
		 */
		config();
		/**
		 * Copy constructor.
		 */
		config(const config &source);
		/**
		 * Copy assignment.
		 */
		config &operator=(const config &source);
		/**
		 * Move constructor.
		 */
		config(config &&source);
		/**
		 * Move assignment.
		 */
		config &operator=(config &&source);

		/**
		 * Add section to this ini configuration.
		 * @param sect section which will be added
		 * @throws ambiguity_exception if section with specified name exists
		 */
		void add_section(const section &sect);
		/**
		 * Create and add section with specified name.
		 * @param section_name section with same name cannot exist in config
		 * @throws ambiguity_exception if section with specified name exists
		 */
		void add_section(const std::string &section_name);
		/**
		 * Remove section from internal sections list.
		 * @param section_name name should exist in section list
		 * @throws not_found_exception if section with given name does not exist
		 */
		void remove_section(const std::string &section_name);

		/**
		 * Add given option to specified section.
		 * @param section_name should exist
		 * @param opt option which will be added to appropriate section
		 * @throws not_found_exception if section with given name does not exist
		 * @throws ambiguity_exception if option with specified name exists
		 */
		void add_option(const std::string &section_name, const option &opt);
		/**
		 * Creates and add option to specified section.
		 * Function add_option(const std::string &section_name, const option &opt)
		 *   is used after option creation.
		 * @param section_name should exist in this config
		 * @param option_name name of newly created option
		 * @param value value which will be stored in new option
		 * @throws ambiguity_exception if option with specified name exists
		 */
		template<typename ValueType>
		void add_option(const std::string &section_name,
			const std::string &option_name, ValueType value)
		{
			option opt(option_name);
			opt.set<ValueType>(value);
			add_option(section_name, opt);
		}

		/**
		 * Removes option with given name from given section.
		 * @param section_name index to section list
		 * @param option_name option with this name will be removed
		 * @throws not_found_exception if section or option
		 * with given name does not exist
		 * @throws not_found_exception if section or option with given name does not exist
		 */
		void remove_option(const std::string &section_name,
			const std::string &option_name);

		/**
		 * Returns size of sections list
		 * @return unsigned integer
		 */
		size_t size() const;
		/**
		 * Access section on specified index.
		 * @param index index of requested value
		 * @return modifiable reference to stored section
		 * @throws not_found_exception if index is out of range
		 */
		section &operator[](size_t index);
		/**
		 * Access constant reference on section on specified index.
		 * @param index index of requested value
		 * @return constant reference to stored section
		 * @throws not_found_exception if index is out of range
		 */
		const section &operator[](size_t index) const;
		/**
		 * Access section with specified name.
		 * @param section_name name of requested section
		 * @return modifiable reference to stored section
		 * @throws not_found_exception if section with given name does not exist
		 */
		section &operator[](const std::string &section_name);
		/**
		 * Access constant reference on section with specified name.
		 * @param section_name name of requested section
		 * @return constant reference to stored section
		 * @throws not_found_exception if section with given name does not exist
		 */
		const section &operator[](const std::string &section_name) const;
		/**
		 * Tries to find section with specified name inside this config.
		 * @parame section_name name which is searched
		 * @return true if section with this name is present, false otherwise
		 */
		bool contains(const std::string &section_name) const;

		/**
		 * Validates this config agains given schema.
		 * @param schm specifies how this config should look like
		 * @param mode validation mode
		 * @return true if this config comply given schema
		 */
		bool validate(const schema &schm, schema_mode mode);

		/**
		 * Equality operator.
		 * @param other
		 * @return
		 */
		bool operator ==(const config &other) const;
		/**
		 * Inequality operator.
		 * @param other
		 * @return
		 */
		bool operator !=(const config &other) const;

		/**
		* Iterator pointing at the beginning of sections list.
		* @return config_iterator
		*/
		iterator begin();
		/**
		* Iterator pointing at the end of sections list.
		* @return config_iterator
		*/
		iterator end();
		/**
		 * Constant iterator pointing at the beginning of sections list.
		 * @return config_iterator
		 */
		const_iterator begin() const;
		/**
		 * Constant iterator pointing at the end of sections list.
		 * @return config_iterator
		 */
		const_iterator end() const;
		/**
		 * Constant iterator pointing at the beginning of sections list.
		 * @return config_iterator
		 */
		const_iterator cbegin() const;
		/**
		 * Constant iterator pointing at the end of sections list.
		 * @return config_iterator
		 */
		const_iterator cend() const;

		/**
		 * Classic stream operator for printing this instance to output stream.
		 * @param os output stream
		 * @return reference to output stream which allows chaining
		 */
		friend std::ostream &operator<<(std::ostream &os, const config &conf);
	};

	std::ostream &operator<<(std::ostream &os, const config &conf);


	/**
	 * Templated config iterator.
	 * Templates provide const and non-const iterator in one implementation.
	 * For easier implementation inheritance from std::iterator is used.
	 */
	template<typename Element>
	class config_iterator
		: public std::iterator<std::random_access_iterator_tag, Element>
	{
	private:
		/** Reference to container which can be iterated */
		config &container_;
		/** Position in iterable container */
		size_t position_;

	public:

		using typename std::iterator<std::random_access_iterator_tag, Element>::reference;
		using typename std::iterator<std::random_access_iterator_tag, Element>::pointer;

		/**
		 * Deleted default constructor.
		 */
		config_iterator() = delete;
		/**
		 * Copy constructor.
		 */
		config_iterator(const config_iterator &src) = default;
		/**
		 * Copy assignment.
		 */
		config_iterator& operator=(const config_iterator &source) = default;
		/**
		 * Move constructor.
		 */
		config_iterator(config_iterator &&src) = default;
		/**
		 * Move assignment.
		 */
		config_iterator& operator=(config_iterator &&source) = default;

		/**
		 * Construct iterator on given container
		 * and pointing at specified postion.
		 * @param source container which can be iterated
		 * @param position initial position to given container
		 */
		config_iterator(config &source, size_t position)
			: container_(source), position_(position) {}
		/**
		 * Construct iterator on given container pointing at the start.
		 * @param source container which can be iterated
		 */
		config_iterator(config &source) : config_iterator(source, 0) {}

		/**
		 * Moves iterator to next position.
		 * @return iterator itself
		 */
		config_iterator &operator++()
		{
			++position_;
			return *this;
		}
		/**
		 * Moves iterator to next position.
		 * @return new iterator with old position
		 */
		config_iterator operator++(int)
		{
			config_iterator old(*this);
			operator++();
			return old;
		}

		/**
		 * Equality compare method for iterators.
		 * @param second
		 * @return true if iterators are the same
		 */
		bool operator==(const config_iterator &second) const
		{
			return &container_ == &second.container_ && position_ == second.position_;
		}
		/**
		 * Non-equality compare method for iterators.
		 * @param second
		 * @return true if iterators are different
		 */
		bool operator!=(const config_iterator &second) const
		{
			return !(*this == second);
		}
		/**
		 * Less than compare method for iterators.
		 * @param second
		 * @return true if this iterator is lesser than second
		 */
		bool operator<(const config_iterator &second) const
		{
			return position_ < second.position_;
		}

		/**
		 * Iterator dereference operator.
		 * @return reference to section on current position
		 */
		reference operator*()
		{
			return *container_.sections_.at(position_);
		}

		/**
		 * Iterator -> operator
		 * @return pointer to option on current position
		 */
		pointer operator->()
		{
			return &(operator*());
		}
	};
}

#endif
