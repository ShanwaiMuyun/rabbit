import { ref, computed } from 'vue'
import { defineStore } from 'pinia'

export const useCartStore = defineStore('cart', () => {
    const cartList = ref([])

    const addCart = (goods) => {
        const item = cartList.value.find((item) => goods.skuId === item.skuId)

        if (item) {
            item.count += goods.count
        } else {
            cartList.value.push(goods)
        }
    }

    const delCart = (skuId) => {
        const idx = cartList.value.findIndex((item) => skuId === item.skuId)
        if (idx > -1) {
            cartList.value.splice(idx, 1)
        }
    }

    const updateNewList = () => {
        return Promise.resolve(cartList.value)
    }

    const clearCart = () => {
        cartList.value = []
    }

    const singleCheck = (skuId, selected) => {
        const item = cartList.value.find((item) => skuId === item.skuId)
        if (item) {
            item.selected = selected
        }
    }

    const allCheck = (selected) => {
        cartList.value.forEach((item) => {
            item.selected = selected
        })
    }

    const allCount = computed(() => cartList.value.reduce((a, c) => a + c.count, 0))
    const allPrice = computed(() => cartList.value.reduce((a, c) => a + c.count * c.price, 0))
    const selectedCount = computed(() => cartList.value.filter((item) => item.selected).reduce((a, c) => a + c.count, 0))
    const selectedPrice = computed(() => cartList.value.filter((item) => item.selected).reduce((a, c) => a + c.count * c.price, 0))
    const isAll = computed(() => cartList.value.length > 0 && cartList.value.every((item) => item.selected))

    return {
        cartList,
        allCount,
        allPrice,
        isAll,
        selectedCount,
        selectedPrice,
        addCart,
        delCart,
        clearCart,
        updateNewList,
        singleCheck,
        allCheck
    }
}, {
    persist: true
})
