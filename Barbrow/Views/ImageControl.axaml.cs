using Avalonia;
using Avalonia.Controls;
using Avalonia.Markup.Xaml;
using Avalonia.Media;

namespace MyApp.Views
{
    public partial class ImageControl : UserControl
    {
        public ImageControl()
        {
            InitializeComponent();
        }

        private void InitializeComponent()
        {
            AvaloniaXamlLoader.Load(this);
        }

        public override void Render(DrawingContext context)
        {
            //var map = DataContext as MapViewModel;
            //Pen pen = new Pen(Brushes.Black);
            //if (map != null)
            //{

            //    foreach (var layer in map.Layers)
            //    {
            //        foreach (var geometry in layer.Geometries)
            //        {
            //            context.DrawGeometry(Brushes.Black, pen, GeometryConverter.ToAvaloniaGeometry(geometry.Geometry));
            //        }
            //    }
            //}
            base.Render(context);
        }
    }
}
