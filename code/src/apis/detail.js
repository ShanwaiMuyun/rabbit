import httpInstance from '@/utils/http'

const toDetail = (item) => ({
    id: item.id,
    name: item.name,
    desc: item.description,
    price: Number(item.price),
    oldPrice: Number(item.price),
    picture: item.image_url,
    stock: Number(item.stock),
    seller: item.seller_username,
    category: item.category || 'others',
    mainPictures: [item.image_url],
    salesCount: 0,
    commentCount: 0,
    collectCount: 0,
    brand: {
        name: item.seller_username
    },
    categories: [
        { id: 'online-all', name: '全部商品' },
        { id: 'online', name: '在线商品' }
    ],
    specs: [],
    skus: [{
        id: String(item.id),
        skuId: String(item.id),
        inventory: Number(item.stock),
        price: Number(item.price),
        oldPrice: Number(item.price),
        specs: []
    }],
    details: {
        properties: [
            { name: '卖家', value: item.seller_username },
            { name: '库存', value: String(item.stock) },
            { name: '审核状态', value: item.status }
        ],
        pictures: [item.image_url]
    }
})

export const getDetail = async (id) => {
    const res = await httpInstance({
        url: '/products',
        params: {
            approvedOnly: 1
        }
    })

    const product = (res.data || []).find((item) => String(item.id) === String(id))

    return {
        result: product ? toDetail(product) : null
    }
}

export const getHotGoodsAPI = async ({ limit = 3 } = {}) => {
    const res = await httpInstance({
        url: '/products'
    })

    return {
        result: (res.data || []).slice(0, limit).map(toDetail)
    }
}
