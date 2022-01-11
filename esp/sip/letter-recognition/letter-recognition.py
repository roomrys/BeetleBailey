import numpy as np
from PIL import Image, ImageOps

# class SPI_Command:
#     'Common base class for all SPI commands'
#     def __init__(self, add)


def find_start_end(data, axis):
    """Finds the indices of the first and last non-zero value of array along specified axis.

    Args:
        data (int64): Numpy array.
        axis (int64): Axis along which to find the indices of the first and last nonzero elements.

    Returns:
        data_start (int64), data_end (int64): Indices of first and last nonzero elements of array along specfied axis.
    """
    data_analysis = (np.sum(data<255, axis)) > 0
    data_start = np.nonzero(data_analysis)[0][0]
    data_end = np.nonzero(data_analysis)[0][-1]
    return data_start, data_end

def find_pad_amount(dividend, divisor):
    quotient_actual = dividend / divisor
    dividend_desired = np.ceil(quotient_actual) * divisor
    pad_amount = int(dividend_desired - dividend)
    return pad_amount

def center_pad(data, pad_amount):
    pad_row = int(np.ceil(pad_amount[0] / 2))
    pad_col = int(np.ceil(pad_amount[1] / 2))
    return np.pad(data, ((pad_row, pad_row), (pad_col, pad_col)), constant_values=255)

def pad_square(data, divisor):
    dim = np.shape(data)
    ((num_max, dim_max), (num_min, dim_min)) = ((dim[0], 0), (dim[1], 1)) if (dim[0] > dim[1]) else ((dim[1], 1), (dim[0], 0))
    pad_max = find_pad_amount(num_max, divisor)
    pad_min = int(np.ceil(num_max / divisor)*divisor - num_min)
    pad_amount = (pad_max, pad_min) if (dim_max < dim_min) else (pad_min, pad_max)
    return center_pad(data, pad_amount)

def ensure_size_correct(data, divisor):
    nrow, ncol = np.shape(data)
    try:
        assert nrow == ncol, f'nrow = {nrow} is not equal to ncol = {ncol}'
    except:
        dim_min = nrow if (nrow<ncol) else ncol
        data = data[0:dim_min, 0:dim_min]
    try: 
        assert nrow%divisor == 0, f'nrow = {nrow} is not divisible by divisor = {divisor}'
    except:
        residual = nrow%divisor
        data = data[0:nrow-residual, 0:nrow-residual]
    return data


def classify_binary_block(block, limit):
    num_elements = np.prod(np.shape(block))
    num_ones = np.sum(block)
    percent_ones = num_ones / num_elements
    classification = 1 if (percent_ones > limit) else 0
    return classification

def draw_block_lines(data, divisor):
    pixels_per_block = int(np.ceil(np.shape(data)[0] / divisor))
    data_copy = data.copy()
    data_copy[::pixels_per_block, :] = 100
    data_copy[:, ::pixels_per_block] = 100
    return data_copy

def get_digits_dict(block_mat):
    digits = {}
    bmat = block_mat.copy()
    ones_idx = np.where(bmat > 0)
    ones_idx_unique_row = np.unique(ones_idx[0])
    prev_idx = 0
    for col in ones_idx_unique_row:
        last_idx = (np.where(ones_idx[0] == col)[0])[-1] + 1
        digits[col] = ones_idx[1][prev_idx:last_idx]
        prev_idx = last_idx
    return digits.copy()

divisor = 8
limit = 0.7

# 1. read in image as binary array
base_name = 'percent'
image = ImageOps.grayscale(Image.open('./assets/' + base_name + '.png'))
data = np.asarray(image)

# 2. crop image
# find first nonzero row
row_start, row_end = find_start_end(data, 1)
col_start, col_end = find_start_end(data, 0)
# crop image
data = data[row_start:row_end, col_start:col_end]
print(np.shape(data))
# convert array to image
image_cropped = Image.fromarray(data)

# 3. pad (& center) image so that dimensions are square and divisible by 8
data = pad_square(data, divisor)
data = ensure_size_correct(data, divisor)
print(np.shape(data))
image_square = Image.fromarray(draw_block_lines(data, divisor)).save('./assets/'+ base_name +'_square.png')

# 4. split image into 8 x 8 with blocks
pixels_image = np.shape(data)[0]
pixels_per_block = int(pixels_image / divisor)
# convert data to ones and zeros
mean = (np.max(data) - np.min(data)) / 2
data[data < mean] = 0
data[data >= mean] = 1
# create block matrix for classification purposes
block_mat = np.zeros((divisor, divisor))
block_mat_full = np.zeros_like(data)

# 5. classify each block as 1 or 0
for row_num in range(0, divisor):
    for col_num in range(0, divisor):
        block = data[row_num*pixels_per_block:(row_num+1)*pixels_per_block, col_num*pixels_per_block:(col_num+1)*pixels_per_block]
        zero_or_one = classify_binary_block(block, limit)
        block_mat_full[row_num*pixels_per_block:(row_num+1)*pixels_per_block, col_num*pixels_per_block:(col_num+1)*pixels_per_block] = zero_or_one
        block_mat[row_num, col_num] = zero_or_one
# (Image.fromarray(block_mat*255)).show()
image_block = Image.fromarray(block_mat_full*255).save('./assets/' + base_name + '_block.png')
# block_mat = draw_block_lines(block_mat, divisor)

# 6. export key-value pair of letter-image
digits = get_digits_dict(block_mat)
print(digits)

# 7. convert digits to spi commands