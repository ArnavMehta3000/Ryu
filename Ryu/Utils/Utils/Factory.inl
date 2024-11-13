namespace Ryu::Utils
{
	template <class Base, class... Args>
	template <class T>
	bool Factory<Base, Args...>::Registrar<T>::s_registered =
		Factory<Base, Args...>::Registrar<T>::Register();
}
