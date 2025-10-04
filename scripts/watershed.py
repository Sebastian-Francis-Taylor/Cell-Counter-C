import numpy as np
from scipy import ndimage
from scipy.ndimage import label, distance_transform_edt
from skimage.segmentation import watershed
from skimage.feature import peak_local_max
import matplotlib.pyplot as plt
from PIL import Image

def watershed_segment_cells(image_path, min_distance=6, save_outputs=True):
    """
    Watershed segmentation for cell separation.
    
    Args:
        image_path: Path to binary image (cells white, background black)
        min_distance: Minimum distance between cell centers (tune this)
        save_outputs: Whether to save intermediate results
    """
    # Load image
    img = Image.open(image_path).convert('L')
    binary = np.array(img) > 128  # True for cells, False for background
    
    # Distance transform
    distance = distance_transform_edt(binary)
    
    # Find peaks (cell centers) with minimum separation
    local_maxima = peak_local_max(
        distance, 
        min_distance=min_distance,  # Key parameter - increase to reduce over-segmentation
        indices=False,
        exclude_border=False
    )
    
    # Label the peaks
    markers, num_peaks = label(local_maxima)
    print(f"Found {num_peaks} cell centers")
    
    # Watershed
    labels = watershed(-distance, markers, mask=binary)
    
    # Create output showing watershed lines
    watershed_lines = np.zeros_like(binary)
    for i in range(1, labels.max() + 1):
        mask = labels == i
        dilated = ndimage.binary_dilation(mask)
        border = dilated & ~mask
        watershed_lines |= border
    
    # Apply erosion
    result = binary.copy()
    result[watershed_lines] = False
    
    if save_outputs:
        # Visualize results
        fig, axes = plt.subplots(2, 3, figsize=(15, 10))
        
        axes[0, 0].imshow(binary, cmap='gray')
        axes[0, 0].set_title('Original Binary')
        
        axes[0, 1].imshow(distance, cmap='hot')
        axes[0, 1].set_title('Distance Transform')
        
        axes[0, 2].imshow(local_maxima, cmap='gray')
        axes[0, 2].set_title(f'Peaks (min_dist={min_distance})')
        
        axes[1, 0].imshow(labels, cmap='nipy_spectral')
        axes[1, 0].set_title(f'Watershed Labels ({num_peaks} regions)')
        
        axes[1, 1].imshow(watershed_lines, cmap='gray')
        axes[1, 1].set_title('Watershed Lines')
        
        axes[1, 2].imshow(result, cmap='gray')
        axes[1, 2].set_title('Final Result')
        
        for ax in axes.flat:
            ax.axis('off')
        
        plt.tight_layout()
        plt.savefig('watershed_debug.png', dpi=150)
        plt.show()
        
        # Save result
        Image.fromarray((result * 255).astype(np.uint8)).save('watershed_result.png')
    
    return result, labels, distance

# Example usage
if __name__ == "__main__":
    # Test with your image
    result, labels, distance = watershed_segment_cells(
        'stage_0.bmp',  # Your binary image
        min_distance=6  # Start with 6, increase if over-segmented
    )
    
    # Try different min_distance values
    for min_dist in [4, 5, 6, 7, 8]:
        print(f"\nTesting min_distance={min_dist}")
        result, labels, distance = watershed_segment_cells(
            'stage_0.bmp',
            min_distance=min_dist,
            save_outputs=False
        )
        print(f"Regions found: {labels.max()}")
