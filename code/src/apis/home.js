import httpInstance from '@/utils/http'

const toGoods = (item) => ({
    id: item.id,
    name: item.name,
    desc: item.description,
    price: Number(item.price),
    picture: item.image_url,
    stock: Number(item.stock),
    seller: item.seller_username,
    category: item.category || 'others'
})

export function getBannerAPI() {
    return Promise.resolve({
        result: [{
            id: 'local-banner',
            imgUrl: new URL('@/assets/images/login-bg.png', import.meta.url).href
        }]
    })
}

export async function findNewAPI() {
    const res = await httpInstance({
        url: '/products'
    })

    return {
        result: (res.data || []).slice(0, 4).map(toGoods)
    }
}

export async function getHotAPI() {
    const res = await httpInstance({
        url: '/products'
    })

    return {
        result: (res.data || []).slice(0, 4).map(toGoods)
    }
}

export async function getGoodsAPI() {
    const res = await httpInstance({
        url: '/products'
    })

    const goods = (res.data || []).map(toGoods)

    return {
        result: [{
            id: 'local-products',
            name: '在线商品',
            picture: goods[0]?.picture || '',
            saleInfo: '已审核商品',
            goods
        }]
    }
}
