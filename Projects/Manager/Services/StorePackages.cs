using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using Windows.ApplicationModel;
using Windows.Management.Deployment;
using Windows.Storage;
using Windows.Data.Xml.Dom;
using System.Diagnostics;

namespace Manager.Services
{
	class StorePackage {
		public string name { get; set; }
		public string publisher { get; set; }
		public string description { get; set; }
		public string location { get; set; }
		public string id { get; set; }
		public string appendix { get; set; }
		public string propName { get; set; }
		public string bootstrap { get; set; }
		public List<int> assetsScaleList;
		public Dictionary<string, string> assetsMap;

		public void CorrectAssetsPath() {
			if (assetsScaleList.Any()) {
				var maxScaleFactor = assetsScaleList.Max();

				List<string> ids = assetsMap.Keys.ToList();
				foreach (var id in ids) {
					var path = assetsMap[id].Substring(0, assetsMap[id].LastIndexOf("\\"));
					var fileName = path.Substring(path.LastIndexOf("\\"));
					var name = fileName.Substring(0, fileName.LastIndexOf("."));
					var extension = fileName.Substring(fileName.LastIndexOf("."));
					var newFileName = "";

					//assetsMap[id] = location + assetsMap[id];
				}
			}
		}
	}
	public class StorePackages
	{
		public StorePackages() {
			QueryPackages();
		}
		private List<StorePackage> packages;
		public async void QueryPackages() {
			PackageManager packageManager = new PackageManager();

			foreach (var package in packageManager.FindPackagesForUserWithPackageTypes("",PackageTypes.Main)) {
				if (package.Status.VerifyIsOK() && !package.IsDevelopmentMode) {
					PackageId packageId = package.Id;
					try {
						StorageFolder storageFolder		= package.InstalledLocation;
						StorageFile storageFile			= await storageFolder.GetFileAsync("AppxManifest.xml");
						XmlDocument doc					= await XmlDocument.LoadFromFileAsync(storageFile);

						XmlNodeList packageResources	= doc.GetElementsByTagName("Resource");
						XmlNodeList packageVisuals		= doc.GetElementsByTagName("uap:VisualElements");
						XmlNodeList packageTileVisuals	= doc.GetElementsByTagName("uap:DefaultTile");

						StorePackage tempPackage = new StorePackage();

						foreach (var res in packageResources) {
							IXmlNode scale = res.Attributes.GetNamedItem("uap:Scale");
							if (scale == null)
								break;
							tempPackage.assetsScaleList.Add(int.Parse(scale.InnerText));
						}

						if (packageVisuals.Length > 0)
						{
							tempPackage.name = packageVisuals.Item(0).Attributes.GetNamedItem("DisplayName").InnerText;
							tempPackage.description = packageVisuals.Item(0).Attributes.GetNamedItem("Description").InnerText;
							//tempPackage.assetsMap.Add("smallLogo", packageVisuals.Item(0).Attributes.GetNamedItem("Square44x44Logo").InnerText);
							//tempPackage.assetsMap.Add("bigLogo", packageVisuals.Item(0).Attributes.GetNamedItem("Square150x150Logo").InnerText);
							//tempPackage.assetsMap["smallLogo"] = packageVisuals.Item(0).Attributes.GetNamedItem("Square44x44Logo").InnerText;
							//tempPackage.assetsMap["bigLogo"] = packageVisuals.Item(0).Attributes.GetNamedItem("Square150x150Logo").InnerText;
						}
						else {
							tempPackage.name = "N/A";
							tempPackage.description = "N/A";
							//tempPackage.assetsMap["smallLogo"] = "N/A";
							//tempPackage.assetsMap["bigLogo"] = "N/A";
							tempPackage.assetsMap.Add("smallLogo", "N/A");
							tempPackage.assetsMap.Add("bigLogo", "N/A");
						}
						if (packageTileVisuals.Length > 0)
						{
							//tempPackage.assetsMap.Add("wideTile", packageTileVisuals.Item(0).Attributes.GetNamedItem("Wide310x150Logo").InnerText);
							//tempPackage.assetsMap["wideTile"] = packageTileVisuals.Item(0).Attributes.GetNamedItem("Wide310x150Logo").InnerText;
						}
						else {
							//tempPackage.assetsMap["wideTile"] = "N/A";
							tempPackage.assetsMap.Add("wideTile", "N/A");
						}
						tempPackage.location	= storageFolder.Path;
						tempPackage.id			= packageId.FamilyName;

						if (doc.GetElementsByTagName("Application").Length > 0)
							tempPackage.appendix = doc.GetElementsByTagName("Application").Item(0).Attributes.GetNamedItem("Id").InnerText;
						else if (doc.GetElementsByTagName("f:Application").Length > 0)
							tempPackage.appendix = doc.GetElementsByTagName("f:Application").Item(0).Attributes.GetNamedItem("Id").InnerText;
						else
							break;

						if (doc.GetElementsByTagName("DisplayName").Length > 0)
							tempPackage.propName = doc.GetElementsByTagName("DisplayName").Item(0).InnerText;
						else if (doc.GetElementsByTagName("f:DisplayName").Length > 0)
							tempPackage.propName = doc.GetElementsByTagName("f:DisplayName").Item(0).InnerText;
						else
							tempPackage.propName = "N/A";

						if (doc.GetElementsByTagName("PublisherDisplayName").Length > 0)
							tempPackage.publisher = doc.GetElementsByTagName("PublisherDisplayName").Item(0).InnerText;
						else if (doc.GetElementsByTagName("f:PublisherDisplayName").Length > 0)
							tempPackage.publisher = doc.GetElementsByTagName("f:PublisherDisplayName").Item(0).InnerText;
						else
							tempPackage.publisher = "N/A";

						tempPackage.bootstrap	= tempPackage.id + "!" + tempPackage.appendix;

						tempPackage.CorrectAssetsPath();

						packages.Add(tempPackage);
					}
					catch (Exception ex) {

					}
				}
			}
		}
    }
}
