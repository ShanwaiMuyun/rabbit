// 管理用户数据相关
import { defineStore } from 'pinia'
import { ref } from 'vue'
import { loginAPI } from '@/apis/user'
import { useCartStore } from './cartStore'
import { mergeCartAPI } from '@/apis/cart'

export const useUserStore = defineStore('user', () => {
    const cartStore = useCartStore()

    // 1.定义管理用户数据的 state
    const userInfo = ref({})

    // 2.定义获取接口数据的 action 函数
    const getUserInfo = async ({ account, password }) => {
        const res = await loginAPI({ account, password })
        userInfo.value = res.result
        // 合并购物车操作
        await mergeCartAPI(cartStore.cartList.map((item) => {
            return {
                skuId: item.skuId,
                selected: item.selected,
                count: item.count
            }
        }))
        // 获取最新购物车列表
        cartStore.updateNewList()
    }

    // 退出时清除用户信息
    const clearUserInfo = () => {
        userInfo.value = {}
        // 执行清除购物车的 action
        cartStore.clearCart()
    }

    // 3.以对象的格式把 state 和 action return 出去
    return {
        userInfo,
        getUserInfo,
        clearUserInfo
    }
}, {
    persist: true
})