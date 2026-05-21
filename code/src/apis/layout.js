import httpInstance from '@/utils/http'

const toGoods = (item) => ({
    id: item.id,
    name: item.name,
    desc: item.description,
    price: Number(item.price),
    picture: item.image_url
})

export async function getCategoryAPI() {
    const res = await httpInstance({
        url: '/products'
    })

    const goods = (res.data || []).map(toGoods)

    return {
        result: [{
            id: 'local-products',
            name: '在线商品',
            children: [],
            goods
        }]
    }
}
