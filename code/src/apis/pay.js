import httpInstance from '@/utils/http'
import { useUserStore } from '@/stores/userStore'

export const getOrderAPI = async (id) => {
    const userStore = useUserStore()
    const username = userStore.userInfo?.username
    const role = userStore.userInfo?.role || 'buyer'

    const res = await httpInstance({
        url: '/orders',
        params: {
            username,
            role
        }
    })

    const order = (res.data || []).find((item) => String(item.id) === String(id))

    return {
        result: {
            id,
            payMoney: Number(order?.total_price || 0),
            status: order?.status
        }
    }
}
