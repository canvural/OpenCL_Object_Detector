/**
 * Get LBP pattern for 3x3 window around each pixel.
 * 159  85  2
 * 55   49  66
 * 26   10  0
 *
 * This gives 10001011 which is 139 in decimal.
 */

__kernel void get_pattern(
  __read_only image2d_t src,
  __global int *output
)
{
  const sampler_t sampler = CLK_NORMALIZED_COORDS_FALSE | CLK_ADDRESS_CLAMP | CLK_FILTER_NEAREST;

  float4 pixel;
  float2 coord;
  int pattern = 0;
  uchar center;

  int x = get_global_id(0);
  int y = get_global_id(1);

  if(x < 1 || y < 1 || x >= get_image_width(src) - 1 || y >= get_image_height(src) - 1) {
	output[x + (get_image_width(src) * y)] = -1;
    return;
  }

  int block_w = 1,
      block_h = 1;

  // center pixel
  coord = (float2) (x, y);
  pixel = read_imagef(src, sampler, coord);
  center = convert_uchar_sat_rte (pixel.x * 255.0f);

  coord.x -= block_w;
  pixel = read_imagef (src, sampler, coord);
  pattern |= (convert_uchar_sat_rte (pixel.x * 255.0f) > center) << 7;

  coord.y += block_h;
  pixel = read_imagef (src, sampler, coord);
  pattern |= (convert_uchar_sat_rte (pixel.x * 255.0f) > center) << 6;

  coord.x += block_w;
  pixel = read_imagef (src, sampler, coord);
  pattern |= (convert_uchar_sat_rte (pixel.x * 255.0f) > center) << 5;

  coord.x += block_w;
  pixel = read_imagef (src, sampler, coord);
  pattern |= (convert_uchar_sat_rte (pixel.x * 255.0f) > center) << 4;

  coord.y -= block_h;
  pixel = read_imagef (src, sampler, coord);
  pattern |= (convert_uchar_sat_rte (pixel.x * 255.0f) > center) << 3;

  coord.y -= block_h;
  pixel = read_imagef (src, sampler, coord);
  pattern |= (convert_uchar_sat_rte (pixel.x * 255.0f) > center) << 2;

  coord.x -= block_w;
  pixel = read_imagef (src, sampler, coord);
  pattern |= (convert_uchar_sat_rte (pixel.x * 255.0f) > center) << 1;

  coord.x -= block_w;
  pixel = read_imagef (src, sampler, coord);
  pattern |= (convert_uchar_sat_rte (pixel.x * 255.0f) > center) << 0;

  output[x + (get_image_width(src) * y)] = pattern;

}