import httpInstance from '@/utils/http'

export const getProductsAPI = (params = {}) => {
    return httpInstance({
        url: '/products',
        params
    })
}

export const publishProductAPI = (data) => {
    return httpInstance({
        url: '/products',
        method: 'POST',
        data
    })
}

export const updateProductAPI = (data) => {
    return httpInstance({
        url: '/products',
        method: 'PUT',
        data
    })
}

export const deleteProductAPI = ({ productId, seller }) => {
    return httpInstance({
        url: '/products/delete',
        method: 'POST',
        data: {
            productId,
            seller
        }
    })
}
