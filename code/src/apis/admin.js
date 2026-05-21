import httpInstance from '@/utils/http'

export const getReviewUsersAPI = ({ admin, status = 'pending' }) => {
    return httpInstance({
        url: '/admin/users',
        params: {
            admin,
            status
        }
    })
}

export const reviewUserAPI = ({ admin, userId, status }) => {
    return httpInstance({
        url: '/admin/users/review',
        method: 'POST',
        data: {
            admin,
            userId,
            status
        }
    })
}

export const reviewProductAPI = ({ admin, productId, status }) => {
    return httpInstance({
        url: '/admin/products/review',
        method: 'POST',
        data: {
            admin,
            productId,
            status
        }
    })
}
