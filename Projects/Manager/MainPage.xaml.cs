using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Runtime.InteropServices.WindowsRuntime;
using Windows.Foundation;
using Windows.Foundation.Collections;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Controls.Primitives;
using Windows.UI.Xaml.Data;
using Windows.UI.Xaml.Input;
using Windows.UI.Xaml.Media;
using Windows.UI.Xaml.Navigation;
using Manager.Views;

namespace Manager
{
    public sealed partial class MainPage : Page
    {
		private Type currentPage;
		private readonly Dictionary<string, Type> views = new Dictionary<string, Type>() {
			{"settings", typeof(SettingsView) },
			{"store", typeof(StoreView) },
		};
        public MainPage()
        {
            this.InitializeComponent();
        }

		private void Navigation_Loaded(object sender, RoutedEventArgs e)
		{
			ContentFrame.Navigate(views["store"]);
		}

		private void Navigation_Navigate(NavigationViewItem item ) {
			if (item.Tag != null)
			{
				if (currentPage == views[item.Tag.ToString()])
					return;
				currentPage = views[item.Tag.ToString()];
				Navigation.IsBackEnabled = ContentFrame.CanGoBack;
			}
			else {
				if (currentPage == views["settings"])
					return;
				currentPage = views["settings"];
				Navigation.IsBackEnabled = ContentFrame.CanGoBack;
			}

			ContentFrame.Navigate(currentPage);
			Navigation.SelectedItem = item;
		}

		private void Navigation_ItemInvoked(NavigationView sender, NavigationViewItemInvokedEventArgs args)
		{
			if (args.InvokedItem == null)
				return;
			if (args.IsSettingsInvoked)
				Navigation_Navigate((NavigationViewItem)Navigation.SettingsItem);
			else {
				foreach (var item in Navigation.MenuItems.OfType<NavigationViewItem>()) {
					if (args.InvokedItem.Equals(item.Content)){
						Navigation_Navigate(item);
						return;
					}
				}
			}
		}
	}
}
