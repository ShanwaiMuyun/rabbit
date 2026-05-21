import { defineStore } from 'pinia'
import { ref } from 'vue'
import { loginAPI, registerAPI } from '@/apis/user'
import { useCartStore } from './cartStore'

export const useUserStore = defineStore('user', () => {
    const cartStore = useCartStore()
    const userInfo = ref({})

    const getUserInfo = async ({ account, password }) => {
        const res = await loginAPI({ account, password })
        userInfo.value = res.data || {}
        return res
    }

    const register = async ({ account, password, role }) => {
        return registerAPI({ account, password, role })
    }

    const clearUserInfo = () => {
        userInfo.value = {}
        cartStore.clearCart()
    }

    return {
        userInfo,
        getUserInfo,
        register,
        clearUserInfo
    }
}, {
    persist: true
})
