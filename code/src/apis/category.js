import httpInstance from '@/utils/http'

// 获取一级分类列表数据
export function getCategoryAPI(id) {
    return httpInstance({
        url: '/category',
        params: {
            id
        }
    })
}

// 获取二级分类列表数据
export function getCategoryFilterAPI(id) {
    return httpInstance({
        url: '/category/sub/filter',
        params: {
            id
        }
    })
}

/**
 * @description 获取商品列表数据
 * @data {
 *     categoryId: 分类ID,
 *     page: 页码,
 *     pageSize: 每页条数,
 *     sortField: 'publishTime' | 'orderNum' | 'evaluateNum'
 * }
 * @return {*}
 */
export function getSubCategoryAPI(data) {
    return httpInstance({
        url: '/category/goods/temporary',
        method: 'POST',
        data
    })
}