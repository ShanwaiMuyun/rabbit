import httpInstance from '@/utils/http'
import { useCartStore } from '@/stores/cartStore'
import { useUserStore } from '@/stores/userStore'

const defaultAddress = {
    id: 'local-address',
    receiver: '默认收货人',
    contact: '13800000000',
    fullLocation: '默认地区',
    address: '请在订单提交前替换为真实地址',
    isDefault: 0
}

export const getCheckInfoAPI = () => {
    const cartStore = useCartStore()
    const goods = cartStore.cartList
        .filter((item) => item.selected)
        .map((item) => ({
            ...item,
            totalPrice: Number(item.price) * Number(item.count),
            totalPayPrice: Number(item.price) * Number(item.count)
        }))

    const totalPrice = goods.reduce((sum, item) => sum + item.totalPayPrice, 0)

    return Promise.resolve({
        result: {
            userAddresses: [defaultAddress],
            goods,
            summary: {
                goodsCount: goods.reduce((sum, item) => sum + item.count, 0),
                totalPrice,
                postFee: 0,
                totalPayPrice: totalPrice
            }
        }
    })
}

export const createOrderAPI = async () => {
    const cartStore = useCartStore()
    const userStore = useUserStore()
    const buyer = userStore.userInfo?.username
    const selectedGoods = cartStore.cartList.filter((item) => item.selected)

    if (!buyer) {
        throw new Error('请先登录后再下单')
    }

    if (selectedGoods.length === 0) {
        throw new Error('请选择要下单的商品')
    }

    const createdOrders = []

    for (const item of selectedGoods) {
        const res = await httpInstance({
            url: '/orders',
            method: 'POST',
            data: {
                buyer,
                productId: item.id,
                quantity: item.count,
                address: `${defaultAddress.fullLocation} ${defaultAddress.address}`
            }
        })
        createdOrders.push(res.data)
    }

    selectedGoods.forEach((item) => cartStore.delCart(item.skuId))

    return {
        result: {
            id: createdOrders[0]?.orderId,
            orderIds: createdOrders.map((item) => item.orderId)
        }
    }
}
