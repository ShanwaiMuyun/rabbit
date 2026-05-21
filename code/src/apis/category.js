import httpInstance from '@/utils/http'

const toGoods = (item) => ({
    id: item.id,
    name: item.name,
    desc: item.description,
    price: Number(item.price),
    picture: item.image_url,
    stock: Number(item.stock),
    seller: item.seller_username
})

export async function getCategoryAPI() {
    const res = await httpInstance({
        url: '/products'
    })

    const goods = (res.data || []).map(toGoods)

    return {
        result: {
            id: 'local',
            name: '在线商品',
            children: [],
            goods
        }
    }
}

export async function getCategoryFilterAPI() {
    const res = await httpInstance({
        url: '/products'
    })

    const goods = (res.data || []).map(toGoods)

    return {
        result: {
            id: 'local',
            name: '在线商品',
            children: [],
            goods
        }
    }
}

export async function getSubCategoryAPI() {
    const res = await httpInstance({
        url: '/products'
    })

    return {
        result: {
            items: (res.data || []).map(toGoods)
        }
    }
}
