#pragma once

//
//	Created by ABacker on 9/5/2016
//

#ifndef _FAKELIST_ABACKER_NINI_H_
#define _FAKELIST_ABACKER_NINI_H_

#include <string>
#include <assert.h>

#define _NINI_BEGIN namespace nini {
#define _NINI_END }

_NINI_BEGIN

#define DEBUG									0x00011111
#define DEBUG_RANGE_CHECK						0x00000001
#define DEBUG_UNKOWN_ERR_CHECK					0x00000010
#define DEBUG_NEW_DELET_CHECK					0x00000100
#define DEBUG_COUT_ERR_INF						0x00001000
#define DEBUG_PRINT_NODE						0x00010000

#define DEFAULT_SIZE_OF_EACH_NODE				(1000000 / sizeof(_Ty))

#define NOEXCEPT throw()

typedef unsigned int size_type;

template<class _Ty>class FakeList;


//TEMPLATE CLASS _FakeList_Ptr
template<class _Ty>
class _FakeList_Ptr {
public:
	_FakeList_Ptr() : _Data(NULL), _Ref(0) {
	}

	explicit _FakeList_Ptr(_Ty *data) : _Data(data), _Ref(1) {
	}

	~_FakeList_Ptr() {
#if(DEBUG & DEBUG_UNKOWN_ERR_CHECK)
		assert(_Ref == 0);
#endif
		if (_Data)
			delete[] _Data;
	}
	
public:
	_Ty *_Data;
	size_type _Ref;
};


//TEMPLATE CLASS _FakeList_node
template<class _Ty>
class _FakeList_node {

	typedef _FakeList_Ptr<_Ty> ptr;
	typedef _FakeList_node<_Ty> node;

public:
	_FakeList_node()
		: _Size(0), _Offset(0), _Next(NULL), _Ptr(NULL) {
	}

	_FakeList_node(_Ty *_Data, size_type _Size, size_type _Offset = 0, node *_Next = NULL)
		: _Size(_Size), _Offset(_Offset), _Next(_Next), _Ptr(new ptr(_Data)) {
	}

	_FakeList_node(const node &_Node)
		:_Size(_Node._Size), _Offset(_Node._Offset), _Next(_Node._Next), _Ptr(_Node._Ptr) {
		if (this != &_Node && _Ptr)
			++_Ptr->_Ref;
	}

	_FakeList_node(node &&_Node)
		:_Size(_Node._Size), _Offset(_Node._Offset), _Next(_Node._Next), _Ptr(_Node._Ptr) {
		_Node._Tidy();
	}

	node &operator=(const node &_Right) {
		_Dis();

		this->_Size = _Right._Size;
		this->_Offset = _Right._Offset;
		this->_Next = _Right._Next;
		this->_Ptr = _Right._Ptr;

		if (this->_Ptr)
			++(this->_Ptr->_Ref);

		return (*this);
	}

	node &operator=(node &&_Right) {
		_Dis();

		this->_Size = _Right._Size;
		this->_Offset = _Right._Offset;
		this->_Next = _Right._Next;
		this->_Ptr = _Right._Ptr;

		_Right._Tidy();

		return (*this);
	}

	node &assign(_Ty *_Data, size_type _Size, size_type _Offset = 0, node *_Next = NULL) {
		if (_Ptr && (--_Ptr->_Ref == 0)) {
			delete[] _Ptr->_Data;
			this->_Ptr->_Data = _Data;
			this->_Ptr->_Ref = 1;
		}
		else _Ptr = new ptr(_Data);

		this->_Size = _Size;
		this->_Offset = _Offset;
		this->_Next = _Next;

		return (*this);
	}

	node &swap(const node &_Node) {
		std::swap(_Size, _Node._Size);
		std::swap(_Offset, _Node._Offset);
		std::swap(_Next, _Node._Next);
		std::swap(_Ptr, _Node._Ptr);

		return (*this);
	}

	//node *clone(_Ty *(*clone_func)(const _Ty *elem, size_type n)) const {
	//	_Ty *data = clone_func(_Ptr->data, _Size);
	//	return new node(data, _Size, _Offset, _Next);
	//}

	_Ty &operator[](size_type _Pos) {

#if(DEBUG & DEBUG_RANGE_CHECK)
		if (_Pos + 1 > _Size)
			throw std::out_of_range("FakeList");
#endif

		return _Ptr->_Data[_Pos + _Offset];
	}

	const _Ty &operator[](size_type _Pos) const {

#if(DEBUG & DEBUG_RANGE_CHECK)
		if (_Pos + 1 > _Size)
			throw std::out_of_range("FakeList");
#endif

		return _Ptr->_Data[_Pos + _Offset];
	}

	~_FakeList_node() {
		_Dis();
	}

protected:
	void _Tidy() {
		_Size = 0;
		_Offset = 0;
		_Next = NULL;
		_Ptr = NULL;
	}

	void _Dis() {
		if (_Ptr && (--_Ptr->_Ref == 0)) {
			delete _Ptr;
		}
	}

public:
	size_type _Size;
	size_type _Offset;
	node *_Next;

private:
	ptr *_Ptr;
};


//TEMPLATE CLASS _FakeList_const_iterator
template<class _Ty>
class _FakeList_const_iterator {

	typedef _FakeList_node<_Ty> node;
	typedef _FakeList_const_iterator<_Ty> const_iterator;
	friend class FakeList<_Ty>;

public:
	_FakeList_const_iterator()
		:_Cur_pos(0), _Cur_node(NULL) {
	}

	_FakeList_const_iterator(size_type _CurPos, node *_CurNode)
		:_Cur_pos(_CurPos), _Cur_node(_CurNode) {
	}

	const_iterator operator++() {
		if (_Cur_pos + 1 < _Cur_node->_Size) ++_Cur_pos;
		else {
			_Cur_node = _Cur_node->_Next;
			_Cur_pos = 0;

#if(DEBUG & DEBUG_PRINT_NODE)
			printf(",");
#endif
		}
		return (*this);
	}

	const_iterator &operator++(int) {
		const_iterator _Iter = *this;

		++(*this);

		return _Iter;
	}

	const_iterator operator+(size_type _Count) {
		const_iterator _Iter = *this;

		_Iter += _Count;

		return _Iter;
	}

	const_iterator &operator+=(size_type _Count) {
		if (_Cur_pos + _Count >= _Cur_node->_Size) {
			_Count -= (_Cur_node->_Size - _Cur_pos - 1);
			_Cur_pos = 0;
			_Cur_node = _Cur_node->_Next;

			while (_Cur_pos + _Count >= _Cur_node->_Size) {
				_Count -= _Cur_node->_Size;
				_Cur_node = _Cur_node->_Next;
			}
		}
		_Cur_pos += _Count;

		return (*this);
	}

	const _Ty *operator->() const {
		return &((*_Cur_node)[_Cur_pos]);
	}

	const _Ty &operator*() const {
		return (*_Cur_node)[_Cur_pos];
	}

	bool operator==(const const_iterator &_Right) {
		return _Cur_pos == _Right._Cur_pos && _Cur_node == _Right._Cur_node;
	}

	bool operator!=(const const_iterator &_Right) {
		return _Cur_pos != _Right._Cur_pos || _Cur_node != _Right._Cur_node;
	}

protected:
	size_type _Cur_pos;
	node *_Cur_node;
};


//TEMPLATE CLASS _FakeList_iterator
template<class _Ty>
class _FakeList_iterator :public _FakeList_const_iterator<_Ty>{

	typedef _FakeList_node<_Ty> node;
	typedef _FakeList_iterator<_Ty> iterator;
	typedef _FakeList_const_iterator<_Ty> base;
	friend class FakeList<_Ty>;

public:
	_FakeList_iterator()
		:base(){
	}

	_FakeList_iterator(size_type _CurPos, node *_CurNode)
		:base(_CurPos, _CurNode) {
	}

	iterator operator++() {
		++(*static_cast<base *>(this));

		return (*this);
	}

	iterator &operator++(int) {
		iterator _Iter = *this;

		++(*this);

		return _Iter;
	}

	iterator operator+(size_type _Count) {
		iterator _Iter = *this;
		_Iter += _Count;

		return _Iter;
	}

	iterator &operator+=(size_type _Count) {
		*static_cast<base *>(this) += _Count;

		return (*this);
	}

	_Ty *operator->() const {
		return &((*_Cur_node)[_Cur_pos]);
	}

	_Ty &operator*() const {
		return (*_Cur_node)[_Cur_pos];
	}
};


//TEMPLATE CLASS FakeList
template<class _Ty>
class FakeList {
protected:
	typedef _FakeList_node<_Ty> node;

public:
	typedef _FakeList_iterator<_Ty> iterator;
	typedef _FakeList_const_iterator<_Ty> const_iterator;

	//private:
	//	//CAUTION with these two functions!!!
	//	//when:
	//	//  FakeList<char>a("123456");
	//	//  FakeList<char>b = a;
	//	//b shares the same node memory with a.
	//	//you can use b = a.clone() if necessary
	//	FakeList(const FakeList &right)
	//		:_Size(right._Size), _Front(right._Front), _Back(right._Back) {
	//
	////#pragma message(__LOC__"CAUTION with this constructor")
	//
	//		//node *tmp = _Front;
	//		//while (tmp != NULL) {
	//		//	//++tmp->_Ptr->_Ref;
	//
	//		//	tmp = tmp->_Next;
	//		//}
	//	}
	//
	//	FakeList &operator=(const FakeList &right) {
	//
	////#pragma message(__LOC__"CAUTION with this mutator")
	//
	//		//_Size = right._Size;
	//		//_Front = right._Front;
	//		//_Back = right._Back;
	//
	//		//node *tmp = _Front;
	//		//while (tmp != NULL) {
	//		//	//++tmp->_Ptr->_Ref;
	//
	//		//	tmp = tmp->_Next;
	//		//}
	//		return (*this);
	//	}
	//

public:
	FakeList()
		:_Size(0), _Front(NULL), _Back(NULL), _Cow(false) {
	}

	FakeList(const _Ty *_Elem, size_type _Count)
		: _Front(NULL) {
		assign(_Elem, _Count);
	}

	FakeList(_Ty *&&_Elem, size_type _Count)
		: _Front(NULL) {
		assign(std::move(_Elem), _Count);
	}

	FakeList(FakeList &&_Right)
		: _Front(NULL) {
		assign(std::move(_Right));
	}

	FakeList &operator=(FakeList &&_Right) {
		return assign(std::move(_Right));
	}

	FakeList &assign(const _Ty *_Elem, size_type _Count) {
		_Ty *data = _Clone(_Elem, _Count);

		if (_Front == NULL)
			_Front = new node(data, _Count);
		else {
			_Tidy(_Front->_Next);
			_Front->assign(data, _Count);
		}

		_Size = _Count;
		_Back = _Front;
		_Cow = false;

		return *this;
	}

	FakeList &assign(_Ty *&&_Elem, size_type _Count) {
		if (_Front == NULL)
			_Front = new node(_Elem, _Count);
		else {
			_Tidy(_Front->_Next);
			_Front->assign(_Elem, _Count, 0, NULL);
		}

		_Elem = NULL;

		_Size = _Count;
		_Back = _Front;
		_Cow = false;

		return *this;
	}

	/*FakeList &assign(const FakeList &right) {

	}*/

	FakeList &assign(FakeList &&_Right) {
		std::swap(_Size, _Right._Size);
		std::swap(_Front, _Right._Front);
		std::swap(_Back, _Right._Back);
		std::swap(_Cow, _Right._Cow);

		return *this;
	}

	_Ty &operator[](size_type _Pos) {

#if(DEBUG & DEBUG_RANGE_CHECK)
		if (_Pos + 1 > _Size)
			throw std::out_of_range("FakeList");
#endif

		node *_Node = _Front;
		while (_Node != NULL) {
			if (_Node->_Size > _Pos)
				return (*_Node)[_Pos];

			_Pos -= _Node->_Size;
			_Node = _Node->_Next;
		}
		throw std::out_of_range("FakeList");
	}

	const _Ty &operator[](size_type _Pos) const {

#if(DEBUG & DEBUG_RANGE_CHECK)
		if (_Pos + 1 > _Size)
			throw std::out_of_range("FakeList");
#endif
		node *_Node = _Front;
		while (_Node != NULL) {
			if (_Node->_Size > _Pos)
				return (*_Node)[_Pos];

			_Pos -= _Node->_Size;
			_Node = _Node->_Next;
		}
		throw std::out_of_range("FakeList");
	}

	FakeList &insert(const _Ty *_Elem, size_type _Count, size_type _Pos) {

#if(DEBUG & DEBUG_RANGE_CHECK)
		if (_Pos == 0)return push_front(_Elem, _Count);

		if (_Pos > _Size)return append(_Elem, _Count);
#endif
		_Ty *_NewData = _Clone(_Elem, _Count);
		node *_Node = _FindPosNode(&_Pos);

		_Insert(_NewData, _Count, _Node, _Pos);

		_Size += _Count;

		return (*this);
	}

	FakeList &insert(_Ty *&&_Elem, size_type _Count, size_type _Pos) {

#if(DEBUG & DEBUG_RANGE_CHECK)
		if (_Pos == 0)return push_front(_Elem, _Count);

		if (_Pos > _Size)return append(_Elem, _Count);
#endif
		node *_Node = _FindPosNode(&_Pos);

		_Insert(_Elem, _Count, _Node, _Pos);

		_Elem = NULL;

		_Size += _Count;

		return (*this);
	}

	FakeList &insert(const _Ty *_Elem, size_type _Count, iterator _Pos) {

#if(DEBUG & DEBUG_RANGE_CHECK)
		if (_Pos == this->begin())return push_front(_Elem, _Count);

		if (_Pos == this->end())return append(_Elem, _Count);
#endif
		_Ty *_NewData = _Clone(_Elem, _Count);

		_Insert(_NewData, _Count, _Pos._Cur_node, _Pos._Cur_pos);

		_Size += _Count;

		return (*this);
	}

	FakeList &insert(_Ty *&&_Elem, size_type _Count, iterator _Pos) {

#if(DEBUG & DEBUG_RANGE_CHECK)
		if (_Pos == this->begin())return push_front(_Elem, _Count);

		if (_Pos == this->end())return append(_Elem, _Count);
#endif
		_Insert(_Elem, _Count, _Pos._Cur_node, _Pos._Cur_pos);

		_Elem = NULL;

		_Size += _Count;

		return (*this);
	}

	/*FakeList &insert(FakeList &&fakeList,size_type pos) {

	}*/

	FakeList &erase(size_type _Begin, size_type _Count) {

#if(DEBUG &DEBUG_RANGE_CHECK)
		if (_Begin + _Count > _Size)
			throw std::out_of_range("FakeList");
#endif

		if (_Begin == 0 && _Count == _Size) {//erase all
			_Tidy(_Front);
			_Tidy();
		}
		else if (_Begin == 0 && _Count >= _Front->_Size) {//erase left half
			_Count += _Begin;

			node *_Node = _Front;
			node *_Tmp;

			while (_Node != NULL) {
				_Tmp = _Node;
				if (_Node->_Size > _Count) {
					_Front = _Node;
					_Front->_Offset += _Count;
					_Front->_Size -= _Count;
					break;
				}
				_Count -= _Node->_Size;
				_Node = _Node->_Next;

				delete _Tmp;
			}
		}
		else if (_Begin + _Count == _Size&&_Count >= _Back->_Size) {//erase right half
			node *_Node = _FindPosNode(&_Begin);

			_Tidy(_Node->_Next);

			_Node->_Size = _Begin;
			_Node->_Next = NULL;

			_Back = _Node;
		}
		else {//erase middle
			node *_Node = _FindPosNode(&_Begin);
			node *_LHalf = _Node;
			_Count -= _Node->_Size - _Begin;
			_Node->_Size = _Begin;
			_Node = _Node->_Next;

			node *_Tmp;
			while (_Node != NULL) {
				_Tmp = _Node;
				if (_Node->_Size > _Count) {
					_Node->_Offset += _Count;
					_Node->_Size -= _Count;
					_LHalf->_Next = _Node;
					break;
				}
				_Count -= _Node->_Size;
				_Node = _Node->_Next;

				delete _Tmp;
			}
		}

		_Size -= _Count;

		return (*this);
	}

	//FakeList &erase(iterator begin, iterator end) {}
	//FakeList &replace(iterator begin, iterator end, _Ty *val, int n) {}
	//FakeList &replace(size_type begin, size_type end, _Ty *val, size_type n) {}

	FakeList &append(const _Ty *_Elem, size_type _Count) {
		_Ty *_NewData = new _Ty[_Count];
		memcpy(_NewData, _Elem, sizeof(_Ty)*_Count);

		if (_Front == NULL) {
			_Front = new node(_NewData, _Count);
			_Back = _Front;
		}
		else {
			_Back->_Next = new node(_NewData, _Count);
			_Back = _Back->_Next;
		}
		_Size += _Count;

		return (*this);
	}

	FakeList &append(_Ty *&&_Elem, size_type _Count) {
		if (_Front == NULL) {
			_Front = new node(_Elem, _Count);
			_Back = _Front;
		}
		else {
			_Back->_Next = new node(_Elem, _Count);
			_Back = _Back->_Next;
		}

		_Elem = NULL;

		_Size += _Count;

		return (*this);
	}

	FakeList &append(FakeList &&_FakeList) {
		_Back->_Next = _FakeList._Front;
		_Back = _FakeList._Back;

		_FakeList._Front = NULL;
		_FakeList._Back = NULL;

		return (*this);
	}

	FakeList &append(const _Ty &_Elem) {
		return append(&_Elem, 1);
	}

	FakeList &append(_Ty &&_Elem) {
		return append(std::move(&_Elem), 1);
	}

	//FakeList &append(const FakeList &fakeList) {
	//	if (this == &fakeList) {
	//		FakeList tmp = fakeList.clone();
	//		this->_Back = tmp->_Front;
	//	}
	//	else this->_Back = fakeList->_Front;

	//	_Size += fakeList._Size;

	//	return (*this);
	//}

	FakeList &push_front(const _Ty *_Elem, size_type _Count) {
		_Ty *_NewData = new _Ty[_Count];
		memcpy(_NewData, _Elem, sizeof(_Ty)*_Count);

		node *_Node = new node(_NewData, _Count);

		_Node->_Next = _Front;
		_Front = _Node;

		return (*this);
	}

	FakeList &push_front(_Ty *&&_Elem, size_type _Count) {
		node *_Node = new node(_Elem, _Count);

		_Elem = NULL;

		_Node->_Next = _Front;
		_Front = _Node;

		return (*this);
	}

	FakeList &push_back(_Ty *_Elem, size_type _Count) {
		return append(_Elem, _Count);
	}

	FakeList &push_back(const _Ty &_Elem) {
		return append(_Elem);
	}

	FakeList &push_back(_Ty &&_Elem) {
		return append(std::move(_Elem));
	}

	FakeList &pop_front() {
#if(DEBUG & DEBUG_RANGE_CHECK)
		if (_Size == 0)
			throw std::out_of_range("FakeList");
#endif
		if (--_Front->_Size == 0) {
			node *_Tmp = _Front;
			_Front = _Front->_Next;
			delete _Tmp;
		}
		else ++_Front->_Offset;

		if (--_Size == 0)_Back = NULL;

		return (*this);
	}

	FakeList &pop_back() {
#if(DEBUG & DEBUG_RANGE_CHECK)
		if (_Size == 0)
			throw std::out_of_range("FakeList");
#endif
		if (--_Size == 0)_Front = NULL;

		if (--_Back->_Size == 0) {
			delete _Back;
			_Back = _FindNewBack(_Back);
			if (_Back)_Back->_Next = NULL;
		}

		return (*this);
	}

	/*

	FakeList &pop_front_n() {

	return (*this);
	}

	FakeList &pop_back_n() {

	return (*this);
	}

	FakeList &pop_back_node() {

	return (*this);
	}

	FakeList &pop_front_node() {
	#if(DEBUG & DEBUG_RANGE_CHECK)
	if (_Size == 0)
	throw std::out_of_range("FakeList");
	#endif


	return (*this);
	}
	*/

	iterator begin() NOEXCEPT{
		return iterator(0, _Front);
	}

	const_iterator begin()const NOEXCEPT{
		return const_iterator(0, _Front);
	}

	iterator end() NOEXCEPT{
		return iterator(0, NULL);
	}

	const_iterator end() const NOEXCEPT{
		return const_iterator(0, NULL);
	}

	_Ty &front() {
#if(DEBUG & DEBUG_RANGE_CHECK)
		if (_Back == NULL)
			throw std::out_of_range("FakeList");
#endif
		return (*_Front)[0];
	}

	const _Ty &front() const {
#if(DEBUG & DEBUG_RANGE_CHECK)
		if (_Back == NULL)
			throw std::out_of_range("FakeList");
#endif
		return (*_Front)[0];
	}

	_Ty &back() {
#if(DEBUG & DEBUG_RANGE_CHECK)
		if (_Back == NULL)
			throw std::out_of_range("FakeList");
#endif

		return (*_Back)[_Back->_Size - 1];
	}

	const _Ty &back() const {
#if(DEBUG & DEBUG_RANGE_CHECK)
		if (_Back == NULL)
			throw std::out_of_range("FakeList");
#endif
		return (*_Back)[_Back->_Size - 1];
	}

	//FakeList sublist(size_type begin, size_type end) {
	//	return (*this);
	//}

	//FakeList sublist(iterator begin, iterator end) {
	//	return (*this);
	//}

	/*iterator find()const {
		return end();
		}*/

	//DO NOT use it if not necessary
	FakeList clone(size_type _Max_size_of_each_node = DEFAULT_SIZE_OF_EACH_NODE)const {
		FakeList _Ret;

		if (this->_Front) {
			size_type _Num = this->_Size / _Max_size_of_each_node + (this->_Size%_Max_size_of_each_node ? 1 : 0);
			size_type _Len;
			size_type _Rest = this->_Size;

			_Len = (_Rest >= _Max_size_of_each_node ? _Max_size_of_each_node : _Rest);
			_Ret._Front = new node(new char[_Len], _Len);
			_Ret._Back = _Ret._Front;
			_Rest -= _Len;

			while (--_Num) {
				_Len = (_Rest >= _Max_size_of_each_node ? _Max_size_of_each_node : _Rest);
				_Ret._Back->_Next = new node(new char[_Len], _Len);
				_Ret._Back = _Ret._Back->_Next;

				_Rest -= _Len;
			}

			iterator _Des = _Ret.begin();
			for (const_iterator _Src = this->begin(); _Src != this->end(); ++_Src, ++_Des) {
				*_Des = *_Src;
			}

			_Ret._Size = this->_Size;
		}
		return _Ret;
	}

	void swap(FakeList &_FakeList) {
		std::swap(_Size, _FakeList._Size);
		std::swap(_Front, _FakeList._Front);
		std::swap(_Back, _FakeList._Back);
		std::swap(_Cow, _FakeList._Cow);
	}

	void format(size_type _Max_size_of_each_node = DEFAULT_SIZE_OF_EACH_NODE) {
		*this = this->clone(_Max_size_of_each_node);
	}

	size_type size() const {
		return _Size;
	}

	size_type length() const {
		return _Size;
	}

	size_type list_node_length()const {
		size_type _Count = 0;
		node *_Node = _Front;
		while (_Node != NULL) {
			++_Count;
			_Node = _Node->_Next;
		}
		return _Count;
	}

	bool empty() const {
		return _Size == 0;
	}

	void clear() {
		_Tidy(_Front);

		_Size = 0;
		_Front = NULL;
		_Back = NULL;
	}

	~FakeList() {
		this->clear();
	}

protected:
	void _Tidy() {
		_Size = 0;
		_Front = NULL;
		_Back = NULL;
	}

	void _Tidy(node *_First) {
		node *_Tmp;
		while (_First != NULL) {
			_Tmp = _First->_Next;

			delete _First;

			_First = _Tmp;
		}
	}

	void _Insert(_Ty *_Elem, size_type _Count, node *_Node, size_type _Pos) {
		node *_Insert_node = new node(_Elem, _Count);
		if (_Node->_Size == _Pos) {
			_Insert_node->_Next = _Node->_Next;
			_Node->_Next = _Insert_node;
		}
		else {
			node *_Sep_node = new node(*_Node);

			_Sep_node->_Offset = _Pos;
			_Sep_node->_Size = _Node->_Size - _Pos;
			_Node->_Size = _Pos;

			_Sep_node->_Next = _Node->_Next;
			_Node->_Next = _Insert_node;
			_Insert_node->_Next = _Sep_node;
		}
	}

	node *_FindNewBack(node *_Old_back_node) {
		node *_Node = _Front;
		while (_Node != NULL && _Node->_Next != _Old_back_node) {
			_Node = _Node->_Next;
		}
		return _Node;
	}

	node *_FindPosNode(size_type *_Pos) {
		node *_Node = _Front;
		while (_Node != NULL) {
			if (_Node->_Size >= *_Pos) {
				break;
			}
			*_Pos -= _Node->_Size;
			_Node = _Node->_Next;
		}
		return _Node;
	}

	_Ty *_Clone(const _Ty *_Elem, size_type _Count) {
		_Ty *_NewData = new _Ty[_Count];
		_Ty *_pData = _NewData;
		for (size_type i = 0; i < _Count; ++i, ++_pData, ++_Elem)
			*_pData = *_Elem;//mark

		return _NewData;
	}

protected:
	size_type _Size;
	node *_Front;
	node *_Back;
	bool _Cow;
};


//CLASS string_builder
class string_builder :public FakeList<char> {

	typedef FakeList<char> base;

public:
	string_builder()
		:base() {
	}

	//string_builder(const string_builder &str_builder){}

	string_builder(string_builder &&_Str_builder)
		:base(std::move(_Str_builder)) {
	}

	string_builder(const char *_String, size_type _Count)
		:base(_String, _Count) {
	}

	string_builder(char *&&_String, size_type _Count)
		:base(std::move(_String), _Count) {
	}

	explicit string_builder(const char *_String)
		:base(_String, strlen(_String)) {
	}

	explicit string_builder(char *&&_String)
		:base(std::move(_String), strlen(_String)) {
	}

	string_builder &operator=(string_builder &&_Right){
		base::operator=(std::move(_Right));

		return (*this);
	}

	string_builder &append(const char *_String) {
		base::append(_String, strlen(_String));

		return (*this);
	}

	string_builder &append(const char *_String, size_type _Count) {
		base::append(_String, _Count);

		return (*this);
	}

	string_builder &append(char *&&_String) {
		base::append(std::move(_String), strlen(_String));

		return (*this);
	}

	string_builder &append(char *&&_String, size_type _Count) {
		base::append(std::move(_String), _Count);

		return (*this);
	}

	string_builder &append(string_builder &&_Str_builder) {
		base::append(std::move(_Str_builder));

		return (*this);
	}

	std::string to_string() const {
		std::string _Str;

		_Str.resize(_Size + 1);
		_Str.clear();

		node *_Node = _Front;
		while (_Node != NULL) {
			_Str.append(&(*_Node)[0], _Node->_Size);
			_Node = _Node->_Next;
		}

		return _Str;
	}

	string_builder clone() const {
		string_builder _Ret;

		base *pRet = &_Ret;
		pRet->assign(base::clone());

		return _Ret;
	}

	/*char *c_str()const {
		return NULL;
		}*/

	void print(bool _Add_LF = false) const {

#if(DEBUG & DEBUG_PRINT_NODE)
		printf("   ");
#endif

		for (const_iterator _Iter = this->begin(); _Iter != this->end(); ++_Iter)
			printf("%c", *_Iter);

		if (_Add_LF)printf("\n");
	}

	string_builder substr() const {
		return string_builder();
	}

protected:
	static char *_Clone(const char *_String, size_type _Count) {
		char *_New_string = new char[_Count];
		memcpy(_New_string, _String, sizeof(char)*_Count);

		return _New_string;
	}
};

_NINI_END

#endif //_FAKELIST_ABACKER_NINI_H_