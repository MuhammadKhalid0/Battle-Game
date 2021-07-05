#pragma once
template<class T>
class TreeNode
{
private:
	T value;
	int key;
public:
	TreeNode();
	TreeNode(T val, int k);

	int getKey() const;
	void setKey(int k);

	T getVal() const;
	void setVal(T val);

	bool operator> (const TreeNode<T>& T) const;
	bool operator< (const TreeNode<T>& T) const;
	bool operator==(const TreeNode<T>& T) const;

};

template<class T>
inline TreeNode<T>::TreeNode()
{
}

template<class T>
TreeNode<T>::TreeNode(T val, int k)
{
	value = val;
	key = k;
}

template<class T>
int TreeNode<T>::getKey() const
{
	return key;
}

template<class T>
void TreeNode<T>::setKey(int k)
{
	key = k;
}

template<class T>
T TreeNode<T>::getVal() const
{
	return value;
}

template<class T>
inline void TreeNode<T>::setVal(T val)
{
	value = val;
}

template<class T>
bool TreeNode<T>::operator>(const TreeNode<T>& T) const
{
	if (key > T.key)
		return true;
	else
		return false;
}

template <class T>
bool TreeNode<T>::operator<(const TreeNode<T>& T) const
{
	if (key < T.key)
		return true;
	else
		return false;
}

template <class T>
bool TreeNode<T>::operator==(const TreeNode<T>& T) const
{
	if (key == T.key)
		return true;
	else
		return false;
}


