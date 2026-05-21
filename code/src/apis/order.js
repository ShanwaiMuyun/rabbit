import httpInstance from '@/utils/http'

export const getOrdersAPI = ({ username, role = 'buyer' }) => {
    return httpInstance({
        url: '/orders',
        params: {
            username,
            role
        }
    })
}

export const createLocalOrderAPI = ({ buyer, productId, quantity, address }) => {
    return httpInstance({
        url: '/orders',
        method: 'POST',
        data: {
            buyer,
            productId,
            quantity,
            address
        }
    })
}

export const shipOrderAPI = ({ orderId, seller }) => {
    return httpInstance({
        url: '/orders/ship',
        method: 'POST',
        data: {
            orderId,
            seller
        }
    })
}

export const receiveOrderAPI = ({ orderId, buyer }) => {
    return httpInstance({
        url: '/orders/receive',
        method: 'POST',
        data: {
            orderId,
            buyer
        }
    })
}
